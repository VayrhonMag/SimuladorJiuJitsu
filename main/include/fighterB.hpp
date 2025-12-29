#ifndef FIGHTER_B_HPP
#define FIGHTER_B_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"
#include "fighter_state.hpp"
#include "messages.hpp"
#include "technique_loader.hpp"
#include "role_helper.hpp"

using namespace cadmium;

class fighterB : public Atomic<FighterState>
{
public:
    Port<FightUpdate> in;
    Port<FightAction> out;

    fighterB(const std::string &id)
        : Atomic<FighterState>(id, FighterState("fighterB"))
    {
        in = addInPort<FightUpdate>("in");
        out = addOutPort<FightAction>("out");

        state.techniques = loadTechniqueCSV("Grafo.csv");
    }

    void externalTransition(FighterState &s, double) const override
    {
        for (const auto &u : in->getBag())
        {
            std::cout << "[DEBUG][fighterB] Recibí update. Actor: "
                      << u.actor << ", Estado: " << u.my_pos << std::endl;

            // Si el combate terminó
            if (u.finished)
            {
                s.can_think = false;
                s.is_attacker = false;
                std::cout << "[DEBUG][fighterB] Combate terminado. Fin." << std::endl;
                return;
            }

            // Extraer mi posición del estado convertido
            s.position = extractMyPosition(u.my_pos, "fighterB");
            s.opponent = extractMyPosition(u.my_pos, "fighterA");

            // 🔴 INFERIR QUIÉN ES EL ATACANTE (A) EN LA REPRESENTACIÓN
            std::string estado = u.my_pos;
            size_t posA = estado.find("fighterA:");
            size_t posB = estado.find("fighterB:");

            std::string atacante_representacion;
            if (posA < posB)
            {
                // fighterA aparece primero -> fighterA es A (Atacante en la representación)
                atacante_representacion = "fighterA";
            }
            else
            {
                // fighterB aparece primero -> fighterB es A (Atacante en la representación)
                atacante_representacion = "fighterB";
            }

            // Yo soy atacante si:
            // 1. El motor dice que soy el actor ACTUAL
            // 2. Y además soy el atacante en la representación (rol A)
            s.is_attacker = (u.actor == "fighterB") && (atacante_representacion == "fighterB");

            // Yo pienso SOLO si soy el atacante actual
            s.can_think = (u.actor == "fighterB");

            std::cout << "[DEBUG][fighterB] Pos: " << s.position
                      << ", Opp: " << s.opponent
                      << ", Atacante en rep: " << atacante_representacion
                      << ", Soy atacante: " << s.is_attacker
                      << ", CanThink: " << s.can_think << std::endl;
        }
    }

    void internalTransition(FighterState &s) const override
    {
        s.can_think = false;
        s.is_attacker = false;
    }

    // En fighterA.hpp y fighterB.hpp (sección output)
    void output(const FighterState &s) const override
    {
        if (!s.can_think || !s.is_attacker)
            return;

        std::cout << "[DEBUG][" << s.my_name << "] Buscando técnica. Mi posición: ";

        std::vector<const Technique *> valid_techniques;
        std::string patron_busqueda = "A:" + s.position;

        // Buscar técnicas válidas
        for (const auto &t : s.techniques)
        {
            // Verificar si la técnica aplica a mi posición
            size_t pos = t.from_state.find(patron_busqueda);
            bool match = false;

            if (pos != std::string::npos)
            {
                if (pos + patron_busqueda.length() < t.from_state.length())
                {
                    char siguiente = t.from_state[pos + patron_busqueda.length()];
                    match = (siguiente == ',' || siguiente == ')');
                }
                else
                {
                    match = true;
                }
            }

            // Verificar si tengo suficiente energía
            bool has_energy = s.stamina >= t.energy_cost;

            if (match && has_energy)
            {
                valid_techniques.push_back(&t);
            }
        }

        // Si no encuentro como A, buscar como O
        if (valid_techniques.empty())
        {
            std::string patron_oponente = "O:" + s.position;
            for (const auto &t : s.techniques)
            {
                size_t pos = t.from_state.find(patron_oponente);
                bool match = false;

                if (pos != std::string::npos)
                {
                    if (pos + patron_oponente.length() < t.from_state.length())
                    {
                        char siguiente = t.from_state[pos + patron_oponente.length()];
                        match = (siguiente == ',' || siguiente == ')');
                    }
                    else
                    {
                        match = true;
                    }
                }

                bool has_energy = s.stamina >= t.energy_cost;

                if (match && has_energy)
                {
                    valid_techniques.push_back(&t);
                }
            }
        }

        // Seleccionar técnica
        if (!valid_techniques.empty())
        {
            // Seleccionar la técnica más eficiente energéticamente
            const Technique *selected = valid_techniques[0];
            double best_score = 0.0;

            for (const auto *t : valid_techniques)
            {
                // Puntaje: eficiencia energética + velocidad
                double efficiency = (t->energy_cost > 0) ? (double)t->energy_gain / t->energy_cost : 1.0;
                double speed = 1.0 / (t->time_cost + 0.1);
                double score = efficiency * 2.0 + speed;

                // Bonus para técnicas defensivas si la energía es baja
                if (s.stamina < 30 && t->type == "Defensiva")
                {
                    score += 1.5;
                }

                if (score > best_score)
                {
                    best_score = score;
                    selected = t;
                }
            }

            std::cout << "[DEBUG][" << s.my_name << "] Seleccionada: "
                      << selected->name
                      << " (E: " << selected->energy_cost << "/"
                      << selected->energy_gain << " T: " << selected->time_cost << "s)" << std::endl;
            out->addMessage(FightAction(s.my_name, *selected));
        }
        else
        {
            std::cout << "[ERROR][" << s.my_name << "] No encontré técnica válida"
                      << " para posición: " << s.position;
        }
    }

    double timeAdvance(const FighterState &s) const override
    {
        if (s.can_think && s.is_attacker)
        {
            return 0.1;
        }
        return std::numeric_limits<double>::infinity();
    }
};

#endif
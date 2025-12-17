#ifndef FIGHTER_A_HPP
#define FIGHTER_A_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"
#include "fighter_state.hpp"
#include "messages.hpp"
#include "technique_loader.hpp"
#include "role_helper.hpp"

using namespace cadmium;

class fighterA : public Atomic<FighterState>
{
public:
    Port<FightUpdate> in;
    Port<FightAction> out;

    fighterA(const std::string &id)
        : Atomic<FighterState>(id, FighterState("fighterA"))
    {
        in = addInPort<FightUpdate>("in");
        out = addOutPort<FightAction>("out");

        state.techniques = loadTechniqueCSV("Grafo.csv");
    }

    void externalTransition(FighterState &s, double) const override
    {
        for (const auto &u : in->getBag())
        {
            std::cout << "[DEBUG][fighterA] Recibí update. Actor: " 
                      << u.actor << ", Estado: " << u.my_pos << std::endl;
            
            // Si el combate terminó
            if (u.finished) {
                s.can_think = false;
                s.is_attacker = false;
                std::cout << "[DEBUG][fighterA] Combate terminado. Fin." << std::endl;
                return;
            }
            
            // Extraer mi posición del estado convertido
            s.position = extractMyPosition(u.my_pos, "fighterA");
            s.opponent = extractMyPosition(u.my_pos, "fighterB");
            
            // 🔴 INFERIR QUIÉN ES EL ATACANTE (A) EN LA REPRESENTACIÓN
            std::string estado = u.my_pos;
            size_t posA = estado.find("fighterA:");
            size_t posB = estado.find("fighterB:");
            
            std::string atacante_representacion;
            if (posA < posB) {
                // fighterA aparece primero -> fighterA es A (Atacante en la representación)
                atacante_representacion = "fighterA";
            } else {
                // fighterB aparece primero -> fighterB es A (Atacante en la representación)
                atacante_representacion = "fighterB";
            }
            
            // Yo soy atacante si:
            // 1. El motor dice que soy el actor ACTUAL
            // 2. Y además soy el atacante en la representación (rol A)
            s.is_attacker = (u.actor == "fighterA") && (atacante_representacion == "fighterA");
            
            // Yo pienso SOLO si soy el atacante actual
            s.can_think = (u.actor == "fighterA");
            
            std::cout << "[DEBUG][fighterA] Pos: " << s.position 
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

    void output(const FighterState &s) const override
    {
        if (!s.can_think || !s.is_attacker)
            return;

        std::cout << "[DEBUG][fighterA] Buscando técnica. Mi posición: " 
                  << s.position << std::endl;
        
        // 🔴 BUSCAR TÉCNICAS QUE COINCIDAN CON MI POSICIÓN
        // Como soy el atacante (A en la representación), busco técnicas donde A esté en mi posición
        std::string patron_busqueda = "A:" + s.position;
        
        bool encontrada = false;
        for (const auto &t : s.techniques)
        {
            // Verificar si la técnica aplica a mi posición
            // Puede ser que mi posición aparezca como A: o como parte de una cadena más larga
            size_t pos = t.from_state.find(patron_busqueda);
            
            if (pos != std::string::npos) {
                // Verificar que sea exactamente "A:mi_posicion" seguido de coma o paréntesis
                bool match = false;
                if (pos + patron_busqueda.length() < t.from_state.length()) {
                    char siguiente = t.from_state[pos + patron_busqueda.length()];
                    match = (siguiente == ',' || siguiente == ')');
                } else {
                    match = true;  // Final del string
                }
                
                if (match) {
                    std::cout << "[DEBUG][fighterA] ¡ENCONTRADA! " 
                              << t.name << " from=" << t.from_state << std::endl;
                    out->addMessage(FightAction("fighterA", t));
                    encontrada = true;
                    break;
                }
            }
        }
        
        if (!encontrada) {
            // 🔴 INTENTAR BUSCAR COMO OPONENTE (O) también
            // A veces un fighter puede atacar desde la posición de O
            std::string patron_oponente = "O:" + s.position;
            for (const auto &t : s.techniques)
            {
                size_t pos = t.from_state.find(patron_oponente);
                if (pos != std::string::npos) {
                    bool match = false;
                    if (pos + patron_oponente.length() < t.from_state.length()) {
                        char siguiente = t.from_state[pos + patron_oponente.length()];
                        match = (siguiente == ',' || siguiente == ')');
                    } else {
                        match = true;
                    }
                    
                    if (match) {
                        std::cout << "[DEBUG][fighterA] ¡ENCONTRADA como O! " 
                                  << t.name << " from=" << t.from_state << std::endl;
                        out->addMessage(FightAction("fighterA", t));
                        encontrada = true;
                        break;
                    }
                }
            }
        }
        
        if (!encontrada) {
            std::cout << "[ERROR][fighterA] No encontré técnica para posición: " 
                      << s.position << std::endl;
            std::cout << "[ERROR][fighterA] Patrón buscado: " << patron_busqueda << std::endl;
        }
    }

    double timeAdvance(const FighterState &s) const override
    {
        if (s.can_think && s.is_attacker) {
            return 0.1;
        }
        return std::numeric_limits<double>::infinity();
    }
};

#endif
#ifndef FIGHT_ENGINE_HPP
#define FIGHT_ENGINE_HPP

#include <iostream>
#include <cstdlib>
#include <random>

#include "cadmium/modeling/devs/atomic.hpp"
#include "fight_engine_state.hpp"
#include "messages.hpp"
#include "technique.hpp"
#include "role_helper.hpp"

inline double random_between(double min, double max)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

using namespace cadmium;

class fight_engine : public Atomic<FightEngineState>
{
public:
    Port<FightAction> in;
    Port<FightUpdate> outA;
    Port<FightUpdate> outB;

    fight_engine(const std::string &id)
        : Atomic<FightEngineState>(id, FightEngineState())
    {
        in = addInPort<FightAction>("in_action");
        outA = addOutPort<FightUpdate>("outA_update");
        outB = addOutPort<FightUpdate>("outB_update");

        // Inicializar random seed
        srand(time(nullptr));
    }

    void externalTransition(FightEngineState &s, double e) const override
    {
        // Si recibimos una acción, procesarla
        if (!in->empty())
        {
            for (const auto &act : in->getBag())
            {
                std::cout << "[DEBUG][ENGINE] Recibí acción de: "
                          << act.fighter_id
                          << " - " << act.tech.name
                          << " (Tiempo: " << act.tech.time_cost << "s)" << std::endl;

                const Technique &t = act.tech;
                bool success = (rand() % 100) < 50; // 50% éxito

                // Actualizar tiempo total (usando el tiempo de la técnica)
                s.total_time += t.time_cost;

                // Actualizar energía
                if (act.fighter_id == "fighterA")
                {
                    s.energy_fighterA -= t.energy_cost;
                    if (t.type == "Defensiva")
                    {
                        s.energy_fighterA += t.energy_gain;
                        if (s.energy_fighterA > 100)
                            s.energy_fighterA = 100;
                    }
                    if (s.energy_fighterA < 0)
                        s.energy_fighterA = 0;
                }
                else
                {
                    s.energy_fighterB -= t.energy_cost;
                    if (t.type == "Defensiva")
                    {
                        s.energy_fighterB += t.energy_gain;
                        if (s.energy_fighterB > 100)
                            s.energy_fighterB = 100;
                    }
                    if (s.energy_fighterB < 0)
                        s.energy_fighterB = 0;
                }

                if (success)
                {
                    s.global_state = t.to_state_success;
                    s.last_success = true;
                    std::cout << "[DEBUG][ENGINE] ÉXITO -> " << s.global_state
                              << " (Energía A: " << s.energy_fighterA
                              << ", B: " << s.energy_fighterB << ")" << std::endl;
                }
                else
                {
                    s.global_state = t.to_state_fail;
                    s.last_success = false;
                    std::cout << "[DEBUG][ENGINE] FALLO -> " << s.global_state
                              << " (Energía A: " << s.energy_fighterA
                              << ", B: " << s.energy_fighterB << ")" << std::endl;
                }

                s.exchanges++;
                s.last_actor = act.fighter_id;

                // Lógica de turnos considerando energía
                if (success)
                {
                    // Éxito: atacante mantiene iniciativa si tiene energía
                    std::string current_as_A = (act.fighter_id == "fighterA") ? "fighterA" : "fighterB";
                    int current_energy = (current_as_A == "fighterA") ? s.energy_fighterA : s.energy_fighterB;

                    if (current_energy > 20)
                    { // Si tiene suficiente energía
                        s.current_actor = current_as_A;
                    }
                    else
                    {
                        // Cambiar turno por falta de energía
                        s.current_actor = (current_as_A == "fighterA") ? "fighterB" : "fighterA";
                        std::swap(s.actor_A, s.actor_O);
                    }
                }
                else
                {
                    // Fallo: 70% chance que el oponente tome el turno
                    if (rand() % 100 < 70)
                    {
                        s.current_actor = s.actor_O; // Oponente ataca
                        std::swap(s.actor_A, s.actor_O);
                    }
                    else
                    {
                        s.current_actor = s.actor_A; // Atacante mantiene turno
                    }
                }

                std::cout << "[DEBUG][ENGINE] ROLES: A=" << s.actor_A
                          << "(" << s.energy_fighterA << "%)"
                          << ", O=" << s.actor_O
                          << "(" << s.energy_fighterB << "%)" << std::endl;
                std::cout << "[DEBUG][ENGINE] Próximo atacante: " << s.current_actor << std::endl;
                std::cout << "[DEBUG][ENGINE] Tiempo total: " << s.total_time << "s" << std::endl;

                // Verificar fin del combate
                if (s.global_state.find("Sumision") != std::string::npos ||
                    s.global_state.find("Sumisión") != std::string::npos ||
                    s.global_state.find("KO") != std::string::npos ||
                    (s.energy_fighterA <= 0 && s.energy_fighterB <= 0))
                {
                    s.finished = true;
                    std::cout << "[DEBUG][ENGINE] ¡COMBATE TERMINADO!" << std::endl;
                    std::cout << "[DEBUG][ENGINE] Razón: ";
                    if (s.global_state.find("Sumision") != std::string::npos)
                    {
                        std::cout << "Sumisión" << std::endl;
                    }
                    else if (s.energy_fighterA <= 0 && s.energy_fighterB <= 0)
                    {
                        std::cout << "Ambos sin energía" << std::endl;
                    }
                    else if (s.energy_fighterA <= 0)
                    {
                        std::cout << "FighterA sin energía" << std::endl;
                    }
                    else if (s.energy_fighterB <= 0)
                    {
                        std::cout << "FighterB sin energía" << std::endl;
                    }
                    s.current_actor = "none";
                    return;
                }
            }
        }
        else
        {
            // 🔴 INICIAL: Mostrar configuración inicial
            std::cout << "[DEBUG][ENGINE] Inicio. ROLES: A=" << s.actor_A
                      << ", O=" << s.actor_O << std::endl;
            std::cout << "[DEBUG][ENGINE] Energía inicial: A=100%, B=100%" << std::endl;
            std::cout << "[DEBUG][ENGINE] Primer atacante: " << s.current_actor << std::endl;
        }

        s.has_output = true;
    }

    void internalTransition(FightEngineState &s) const override
    {
        s.has_output = false;
    }

    void output(const FightEngineState &s) const override
    {
        if (!s.has_output)
            return;

        // 🔴 Usar el mapeo dinámico de roles
        std::string estado_para_A = convertRoles(s.global_state, s.actor_A, s.actor_O);
        std::string estado_para_B = convertRoles(s.global_state, s.actor_A, s.actor_O);

        // FighterA siempre recibe su perspectiva (rol que ocupa)
        FightUpdate msgA(
            s.current_actor,
            estado_para_A,
            estado_para_A,
            s.last_success,
            s.finished);

        // FighterB siempre recibe su perspectiva
        FightUpdate msgB(
            s.current_actor,
            estado_para_B,
            estado_para_B,
            s.last_success,
            s.finished);

        outA->addMessage(msgA);
        outB->addMessage(msgB);

        std::cout << "[DEBUG][ENGINE] Enviando update. Actor: " << s.current_actor
                  << " | Estado: " << s.global_state << std::endl;
        std::cout << "[DEBUG][ENGINE] ROLES: A=" << s.actor_A << " O=" << s.actor_O << std::endl;
        std::cout << "[DEBUG][ENGINE] Para todos: " << estado_para_A << std::endl;
    }

    double timeAdvance(const FightEngineState &s) const override
    {
        return s.has_output ? 0.0 : std::numeric_limits<double>::infinity();
    }
};

#endif
#ifndef FIGHTER_A_HPP
#define FIGHTER_A_HPP

#include "simple_profiles_tactical.hpp"
#include "profile_manager.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include "cadmium/modeling/devs/atomic.hpp"
#include "fighter_state.hpp"
#include "messages.hpp"
#include "technique_loader.hpp"
#include "role_helper.hpp"

using namespace cadmium;

class fighterA : public Atomic<FighterState>
{
private:
    ProfileManager profileManager;

public:
    Port<FightUpdate> in;
    Port<FightAction> out;

    fighterA(const std::string &id)
        : Atomic<FighterState>(id, FighterState("fighterA"))
    {
        in = addInPort<FightUpdate>("in");
        out = addOutPort<FightAction>("out");

        state.techniques = loadTechniqueCSV("Grafo.csv");
        state.profile = profileManager.getProfile("fighterA");
        
        std::cout << "[INIT][fighterA] Listo. Perfil: " << state.profile.style 
                  << ", Energía inicial: " << state.stamina << std::endl;
    }

    void externalTransition(FighterState &s, double e) const override
    {
        for (const auto &u : in->getBag())
        {
            if (u.finished)
            {
                s.resetAfterAction();
                return;
            }
            s.my_points = u.my_points;
            s.opponent_points = u.opp_points;
            s.time_remaining = u.time_remaining;
            s.total_time = 300.0 - u.time_remaining;
            s.is_winning = (u.my_points > u.opp_points);

            s.position = extractMyPosition(u.my_pos, "fighterA");
            s.opponent = extractMyPosition(u.my_pos, "fighterB");
            s.recordPosition(s.position);
            s.is_attacker = (u.actor == "fighterA");

            if (s.is_attacker && s.phase == FighterState::Phase::IDLE) {

                s.startThinking();
            } else if (!s.is_attacker) {
                s.resetAfterAction();
            }

            s.exchanges++;
        }
    }

    void internalTransition(FighterState &s) const override
    {
        // THINKING
        if (s.phase == FighterState::Phase::THINKING) {
            
            s.thinking_time = 0; // Ya terminó por timeAdvance
            
            // Seleccionar técnica, verifica energia y la actualiza.
            selectTechnique(s);
        }

    }

    void output(const FighterState &s) const override
    {
        if (s.phase == FighterState::Phase::READY_TO_OUTPUT && s.selected_technique != nullptr) {
            std::cout << "[OUTPUT][fighterA] ENVIANDO ACCIÓN: " 
                      << s.selected_technique->name 
                      << " (Energía restante: " << s.stamina << ")" << std::endl;
            
            out->addMessage(FightAction(s.my_name, *(s.selected_technique)));
            
            // 🔴 Resetear después de enviar
            const_cast<FighterState&>(s).resetAfterAction();
        }
    }

    double timeAdvance(const FighterState &s) const override
    {
        
        switch (s.phase) {
            case FighterState::Phase::IDLE:
                // Estado pasivo: espera indefinida por input
                return std::numeric_limits<double>::infinity();
                
            case FighterState::Phase::THINKING:
                // Está pensando: transición cuando termine
                return s.thinking_time;
                
            case FighterState::Phase::READY_TO_OUTPUT:
                // Listo para output: transición inmediata
                return 0.0;
                
            default:
                return std::numeric_limits<double>::infinity();
        }
    }

private:

    void selectTechnique(FighterState &s) const
    {
        std::vector<const Technique *> valid_techniques;
        
        
        std::string patron_A = "A:" + s.position;
        for (const auto &t : s.techniques)
        {
            size_t pos = t.from_state.find(patron_A);
            bool match = false;
            
            if (pos != std::string::npos) {
                if (pos + patron_A.length() < t.from_state.length()) {
                    char siguiente = t.from_state[pos + patron_A.length()];
                    match = (siguiente == ',' || siguiente == ')');
                } else {
                    match = true;
                }
            }
            
            // 🔴 MODIFICADO: Usar el método del estado para verificar energía
            bool has_energy = s.canAffordTechnique(t);
            
            if (match && has_energy) {
                valid_techniques.push_back(&t);
            }
        }

        if (valid_techniques.empty()) {
            std::string patron_O = "O:" + s.position;
            for (const auto &t : s.techniques) {
                size_t pos = t.from_state.find(patron_O);
                bool match = false;
                
                if (pos != std::string::npos) {
                    if (pos + patron_O.length() < t.from_state.length()) {
                        char siguiente = t.from_state[pos + patron_O.length()];
                        match = (siguiente == ',' || siguiente == ')');
                    } else {
                        match = true;
                    }
                }
                
                bool has_energy = s.canAffordTechnique(t);
                
                if (match && has_energy) {
                    valid_techniques.push_back(&t);
                }
            }
        }
        
        if (!valid_techniques.empty()) {
            const Technique *selected = selectTacticalWithProfile(
                valid_techniques,
                s.position,
                &s.profile,
                "aggressive",
                s.is_winning,
                s.stamina,
                s.recent_techniques,
                s.recent_positions
            );
            
            if (selected) {
                std::cout << "[SELECT][fighterA] Técnica seleccionada: " 
                          << selected->name << " (Costo energía: " << selected->energy_cost << ")" << std::endl;
                s.finishThinking(selected); // 🔴 Ahora aquí se gestiona la energía
            } else {
                std::cout << "[ERROR][fighterA] No se pudo seleccionar técnica" << std::endl;
                s.resetAfterAction();
            }
        } else {
            std::cout << "[ERROR][fighterA] No hay técnicas válidas para posición: " 
                      << s.position << " o sin energía suficiente (Energía actual: " << s.stamina << ")" << std::endl;
            s.resetAfterAction();
        }
    }
};

#endif
#ifndef ARBITER_SMART_HPP
#define ARBITER_SMART_HPP

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "cadmium/modeling/devs/atomic.hpp"
#include "messages.hpp"

using namespace cadmium;

struct ArbiterSmartState {
    bool ready = false;
    std::vector<FightAction> actions;
    int energy_fighterA = 100;
    int energy_fighterB = 100;
    
    ArbiterSmartState() {
        srand(static_cast<unsigned int>(time(nullptr)));
    }
    
    // Método para actualizar energía
    void updateEnergy(const FightAction& action) {
        if(action.fighter_id == "fighterA") {
            energy_fighterA -= action.tech.energy_cost;
            if(action.tech.type == "Defensiva") {
                energy_fighterA += action.tech.energy_gain;
                if(energy_fighterA > 100) energy_fighterA = 100;
            }
            if(energy_fighterA < 0) energy_fighterA = 0;
        } else {
            energy_fighterB -= action.tech.energy_cost;
            if(action.tech.type == "Defensiva") {
                energy_fighterB += action.tech.energy_gain;
                if(energy_fighterB > 100) energy_fighterB = 100;
            }
            if(energy_fighterB < 0) energy_fighterB = 0;
        }
    }
};

// Función de impresión para ArbiterSmartState (FUERA de la clase)
inline std::ostream& operator<<(std::ostream& os, const ArbiterSmartState& s) {
    os << "[ArbiterSmartState ready=" << s.ready
       << ", actions_count=" << s.actions.size()
       << ", energy_A=" << s.energy_fighterA
       << ", energy_B=" << s.energy_fighterB
       << "]";
    return os;
}

class arbiter_smart : public Atomic<ArbiterSmartState> {
public:
    Port<FightAction> inA;
    Port<FightAction> inB;
    Port<FightAction> outAction;
    
    arbiter_smart(const std::string& id) : Atomic<ArbiterSmartState>(id, ArbiterSmartState()) {
        inA = addInPort<FightAction>("inA");
        inB = addInPort<FightAction>("inB");
        outAction = addOutPort<FightAction>("outAction");
    }
    
    void externalTransition(ArbiterSmartState& s, double) const override {
        // Recolectar todas las acciones pendientes
        for(const auto& a : inA->getBag()) {
            s.actions.push_back(a);
        }
        for(const auto& a : inB->getBag()) {
            s.actions.push_back(a);
        }
        
        if(!s.actions.empty()) {
            s.ready = true;
        }
    }
    
    void internalTransition(ArbiterSmartState& s) const override {
        s.actions.clear();
        s.ready = false;
    }
    
    void output(const ArbiterSmartState& s) const override {
        if(!s.ready || s.actions.empty()) return;
        
        // 🔥 DECISIÓN INTELIGENTE basada en energía
        std::vector<FightAction> valid_actions;
        
        // Filtrar acciones por energía disponible
        for(const auto& action : s.actions) {
            int current_energy = (action.fighter_id == "fighterA") 
                                ? s.energy_fighterA 
                                : s.energy_fighterB;
            
            if(current_energy >= action.tech.energy_cost) {
                valid_actions.push_back(action);
            } else {
                std::cout << "[ARBITER_SMART] " << action.fighter_id 
                          << " no tiene suficiente energía para " 
                          << action.tech.name 
                          << " (necesita " << action.tech.energy_cost 
                          << ", tiene " << current_energy << ")" << std::endl;
            }
        }
        
        if(valid_actions.empty()) {
            std::cout << "[ARBITER_SMART] Ninguna acción válida por falta de energía" << std::endl;
            return;
        }
        
        // Decisión basada en múltiples factores
        FightAction selected;
        if(valid_actions.size() == 1) {
            // Solo una opción
            selected = valid_actions[0];
        } else {
            // Elegir la mejor técnica basada en:
            // 1. Eficiencia energética (mayor ganancia/costo)
            // 2. Tiempo de ejecución
            // 3. Tipo de técnica (defensivas tienen prioridad cuando la energía es baja)
            
            // Primero, calcular puntaje para cada acción
            std::vector<std::pair<FightAction, double>> scored_actions;
            
            for(const auto& action : valid_actions) {
                double score = 0.0;
                int current_energy = (action.fighter_id == "fighterA") 
                                    ? s.energy_fighterA 
                                    : s.energy_fighterB;
                
                // Factor 1: Eficiencia energética
                double efficiency = 0.0;
                if(action.tech.energy_cost > 0) {
                    efficiency = static_cast<double>(action.tech.energy_gain) / action.tech.energy_cost;
                }
                
                // Factor 2: Velocidad (menor tiempo es mejor)
                double speed_factor = 1.0 / (action.tech.time_cost + 0.1);
                
                // Factor 3: Prioridad de defensa si energía baja
                double defense_bonus = 0.0;
                if(current_energy < 30 && action.tech.type == "Defensiva") {
                    defense_bonus = 2.0; // Bonus para técnicas defensivas cuando la energía es baja
                }
                
                // Factor 4: Penalización por bajo stamina
                double stamina_penalty = (100.0 - current_energy) / 100.0;
                
                score = efficiency * 3.0 + speed_factor * 2.0 + defense_bonus - stamina_penalty;
                
                scored_actions.push_back({action, score});
            }
            
            // Seleccionar la acción con mayor puntaje
            auto best_action = *std::max_element(scored_actions.begin(), scored_actions.end(),
                [](const auto& a, const auto& b) {
                    return a.second < b.second;
                });
            
            selected = best_action.first;
        }
        
        // Actualizar energía en el estado
        const_cast<ArbiterSmartState&>(s).updateEnergy(selected);
        
        outAction->addMessage(selected);
        
        std::cout << "\n[ARBITER_SMART] Decisión inteligente:" << std::endl;
        std::cout << "-------------------------" << std::endl;
        for(size_t i = 0; i < s.actions.size(); i++) {
            std::cout << "Opción " << i << ": " << s.actions[i].fighter_id 
                      << " - " << s.actions[i].tech.name 
                      << " (E: " << s.actions[i].tech.energy_cost << "/" 
                      << s.actions[i].tech.energy_gain << ")" << std::endl;
        }
        std::cout << "🏆 SELECCIONADA: " << selected.fighter_id 
                  << " - " << selected.tech.name 
                  << " (Tiempo: " << selected.tech.time_cost << "s)" << std::endl;
        std::cout << "Energía actual - A: " << s.energy_fighterA 
                  << " B: " << s.energy_fighterB << std::endl;
        std::cout << "-------------------------\n" << std::endl;
    }
    
    double timeAdvance(const ArbiterSmartState& s) const override {
        return s.ready ? 0.0 : std::numeric_limits<double>::infinity();
    }
};

#endif
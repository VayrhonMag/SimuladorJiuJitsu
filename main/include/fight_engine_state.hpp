#ifndef FIGHT_ENGINE_STATE_HPP
#define FIGHT_ENGINE_STATE_HPP

#include <string>
#include <ostream>
#include <cstdlib>  // Para rand()
#include <ctime>    // Para time()

struct FightEngineState
{
    bool finished = false;
    int exchanges = 0;
    std::string global_state = "Pos(A:De_Pie, O:De_Pie)";

    // 🔴 MAPEO DE ROLES: quién es A (Atacante) y O (Oponente) en este momento
    std::string actor_A = "fighterA"; // Quién representa el rol A
    std::string actor_O = "fighterB"; // Quién representa el rol O

    // Para decidir quién ataca
    double think_time_A = 0.0;
    double think_time_B = 0.0;
    
    // Tiempo total transcurrido
    double total_time = 0.0;

    bool has_output = true;
    bool last_success = false;
    std::string current_actor = ""; // Quién ataca AHORA
    std::string last_actor = "";    // Quién atacó la última vez
    bool thinking = false;
    
    // Energía de los luchadores (para que el motor la tenga en cuenta)
    int energy_fighterA = 100;
    int energy_fighterB = 100;

    FightEngineState()
    {
        // Inicializar random seed
        srand(static_cast<unsigned int>(time(nullptr)));
        
        // Inicial: 50% chance cada uno empieza como atacante
        if (rand() % 2 == 0)
        {
            // fighterA es A (Atacante), fighterB es O (Oponente)
            actor_A = "fighterA";
            actor_O = "fighterB";
            current_actor = "fighterA";
        }
        else
        {
            // fighterB es A (Atacante), fighterA es O (Oponente)
            actor_A = "fighterB";
            actor_O = "fighterA";
            current_actor = "fighterB";
        }
    }
};

inline std::ostream &operator<<(std::ostream &os, const FightEngineState &s)
{
    os << "{finished:" << s.finished
       << ", exchanges:" << s.exchanges
       << ", global_state:" << s.global_state
       << ", total_time:" << s.total_time
       << ", energy_A:" << s.energy_fighterA
       << ", energy_B:" << s.energy_fighterB
       << "}";
    return os;
}

#endif
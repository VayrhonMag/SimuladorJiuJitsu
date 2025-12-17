#ifndef FIGHTER_STATE_HPP
#define FIGHTER_STATE_HPP

#include <string>
#include <vector>
#include <ostream>
#include "technique.hpp"

struct FighterState
{
    std::string my_name;     // "fighterA" o "fighterB"
    std::string opponent_name; // Opuesto
    
    std::string position;    // Mi posición actual
    std::string opponent;    // Posición del oponente
    bool is_attacker = false;
    bool can_think = false;

    int stamina = 100;
    int points = 0;
    bool submitted = false;

    std::vector<Technique> techniques;

    FighterState() = default;
    
    // Constructor con nombre
    FighterState(const std::string& name) : my_name(name) {
        if (name == "fighterA") {
            opponent_name = "fighterB";
        } else {
            opponent_name = "fighterA";
        }
    }
};

inline std::ostream &operator<<(std::ostream &os, const FighterState &s)
{
    os << "[FighterState name=" << s.my_name
       << ", pos=" << s.position
       << ", opp=" << s.opponent
       << ", can_think=" << s.can_think
       << ", is_attacker=" << s.is_attacker
       << "]";
    return os;
}

#endif
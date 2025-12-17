#pragma once
#include <string>

struct FighterState {
    std::string new_position;
    int energy_cost;
    bool submission = false;
    int points = 0;
};
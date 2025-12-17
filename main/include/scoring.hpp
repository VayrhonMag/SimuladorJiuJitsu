#pragma once

class Scoring {
public:
    int pointsA = 0;
    int pointsB = 0;

    void add_points(char fighter, int pts) {
        if (fighter == 'A') pointsA += pts;
        else pointsB += pts;
    }
};
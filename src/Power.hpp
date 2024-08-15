#pragma once

#include <ostream>
#include <vector>

struct Energy {
    unsigned long startTick;
    unsigned long endTick;
    double energy;
    bool dynamic;

    friend std::ostream& operator<< (std::ostream& os, const Energy& e);
};

double accumulateEnergy(const std::vector<Energy>& energies);
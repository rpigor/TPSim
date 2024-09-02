#pragma once

#include <ostream>
#include <vector>
#include <string>

struct Energy {
    unsigned long startTick;
    unsigned long endTick;
    double energy;
    std::string instance;
    bool dynamic;

    friend std::ostream& operator<< (std::ostream& os, const Energy& e);
};

namespace Power {

    double accumulateEnergy(const std::vector<Energy>& energies);
    double accumulateDynamicEnergy(const std::vector<Energy>& energies);
    double accumulateStaticEnergy(const std::vector<Energy>& energies);

}

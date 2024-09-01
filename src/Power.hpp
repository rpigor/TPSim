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

namespace Power {

    double accumulateEnergy(const std::vector<Energy>& energies);
    double accumulateDynamicEnergy(const std::vector<Energy>& energies);
    double accumulateStaticEnergy(const std::vector<Energy>& energies);

}

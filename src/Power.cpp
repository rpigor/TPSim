#include "Power.hpp"
#include <cmath>

std::ostream& operator<< (std::ostream& os, const Energy& e) {
    os << "Energy(" << e.energy << ", " << e.startTick << ", " << e.endTick << ", " << e.dynamic << ")";
    return os;
}

double Power::accumulateEnergy(const std::vector<Energy>& energies) {
    double energy = 0.0;
    for (auto& e : energies) {
        energy += fabs(e.energy);
    }
    return energy;
}

double Power::accumulateDynamicEnergy(const std::vector<Energy>& energies) {
    double energy = 0.0;
    for (auto& e : energies) {
        if (!e.dynamic) {
            continue;
        }
        energy += fabs(e.energy);
    }
    return energy;
}

double Power::accumulateStaticEnergy(const std::vector<Energy>& energies) {
    double energy = 0.0;
    for (auto& e : energies) {
        if (e.dynamic) {
            continue;
        }
        energy += fabs(e.energy);
    }
    return energy;
}
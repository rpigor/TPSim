#include "Power.hpp"

std::ostream& operator<< (std::ostream& os, const Energy& e) {
    os << "Energy(" << e.energy << ", " << e.startTick << ", " << e.endTick << ", " << e.dynamic << ")";
    return os;
}

double accumulateEnergy(const std::vector<Energy>& energies) {
    double energy = 0.0;
    for (auto& e : energies) {
        energy += abs(e.energy);
    }
    return energy;
}
#include "Units.hpp"
#include <iostream>

double Units::tickToTime(unsigned long tick, const std::string& timeUnit, const std::string& tickUnit) {
    double factor = static_cast<double>(Units::unitScale(timeUnit)) / static_cast<double>(Units::unitScale(tickUnit));
    return factor*tick;
}

unsigned long Units::timeToTick(double time, const std::string& timeUnit, const std::string& tickUnit) {
    unsigned long factor = Units::unitScale(tickUnit) / Units::unitScale(timeUnit);
    return static_cast<unsigned long>(time*factor);
}

unsigned long Units::unitScale(const std::string& unit) {
    if (unitScales.count(unit.at(0)) != 0) {
        return unitScales.at(unit.at(0));
    }
    return 1e0;
}

bool Units::isValidUnit(const std::string& unit) {
    return unitScales.count(unit.at(0)) != 0;
}

#include "Units.hpp"
#include <iostream>

double Units::tickToTime(unsigned long tick, const std::string& timeUnit, const std::string& tickUnit) {
    double factor = Units::unitScale(tickUnit) / Units::unitScale(timeUnit);
    return factor*tick;
}

unsigned long Units::timeToTick(double time, const std::string& timeUnit, const std::string& tickUnit) {
    unsigned long factor = static_cast<unsigned long>(Units::unitScale(timeUnit) / Units::unitScale(tickUnit));
    return static_cast<unsigned long>(time*factor);
}

double Units::unitScale(const std::string& unit) {
    if (unitScales.count(unit.at(0)) != 0) {
        return unitScales.at(unit.at(0));
    }
    return 1;
}

bool Units::isValidUnit(const std::string& unit) {
    return unitScales.count(unit.at(0)) != 0;
}

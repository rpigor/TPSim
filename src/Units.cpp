#include "Units.hpp"

unsigned long Units::unitScale(const std::string& unit) {
    if (unitScales.count(unit.at(0)) != 0) {
        return unitScales.at(unit.at(0));
    }
    return 1e0;
}

bool Units::isValidUnit(const std::string& unit) {
    return unitScales.count(unit.at(0)) != 0;
}

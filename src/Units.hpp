#pragma once

#include <string>
#include <map>

namespace Units {

    static const std::map<char, unsigned long> unitScales = {
        {'m', 1e3},
        {'u', 1e6},
        {'n', 1e9},
        {'p', 1e12},
        {'f', 1e15},
        {'a', 1e18}
    };

    double tickToTime(unsigned long tick, const std::string& timeUnit, const std::string& tickUnit);
    unsigned long timeToTick(double time, const std::string& timeUnit, const std::string& tickUnit);
    unsigned long unitScale(const std::string& unit);
    bool isValidUnit(const std::string& unit);

}
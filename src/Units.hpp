#pragma once

#include <string>
#include <map>

namespace Units {

    static const std::map<char, double> unitScales = {
        {'E', 1e18},
        {'P', 1e15},
        {'T', 1e12},
        {'G', 1e9},
        {'M', 1e6},
        {'k', 1e3},
        {'m', 1e-3},
        {'u', 1e-6},
        {'n', 1e-9},
        {'p', 1e-12},
        {'f', 1e-15},
        {'a', 1e-18}
    };

    double tickToTime(unsigned long tick, const std::string& timeUnit, const std::string& tickUnit);
    unsigned long timeToTick(double time, const std::string& timeUnit, const std::string& tickUnit);
    double unitScale(const std::string& unit);
    bool isValidUnit(const std::string& unit);

}
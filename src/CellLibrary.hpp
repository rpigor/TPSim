#pragma once

#include "Cell.hpp"
#include <unordered_map>
#include <string>

struct CellLibrary {
    std::string name;
    double voltage;
    std::string timeUnit;
    std::string voltageUnit;
    std::string capacitanceUnit;
    std::string leakagePowerUnit;
    std::unordered_map<std::string, Cell> cells;
};
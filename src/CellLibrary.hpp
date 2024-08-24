#pragma once

#include "Cell.hpp"
#include <unordered_map>
#include <string>

struct CellLibrary {
    std::string name;
    std::string corner;
    double vdd;
    double temperature;
    std::unordered_map<std::string, Cell> cells;
};
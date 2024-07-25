#pragma once

#include "Cell.hpp"

std::unordered_map<Arc, TimingLUT> invDelayAndSlew = {
    { {"a", "z"}, {} }
};
Cell inv = {"INV", {"a"}, {"z"}, {"!a"}, {}, invDelayAndSlew};

std::unordered_map<Arc, TimingLUT> andDelayAndSlew = {
    { {"a", "z"}, {} },
    { {"b", "z"}, {} }
};
Cell and2 = {"AND2", {"a", "b"}, {"z"}, {"a&b"}, {}, andDelayAndSlew};

std::unordered_map<Arc, TimingLUT> nandDelayAndSlew = {
    { {"a", "z"}, {} },
    { {"b", "z"}, {} }
};
Cell nand2 = {"NAND2", {"a", "b"}, {"z"}, {"!(a&b)"}, {}, nandDelayAndSlew};

std::unordered_map<std::string, Cell> cellLib =
{
    {inv.name, inv},
    {and2.name, and2},
    {nand2.name, nand2},
};
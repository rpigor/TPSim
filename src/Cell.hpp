#pragma once

#include <boost/container_hash/hash.hpp>
#include <string>
#include <vector>
#include <unordered_map>

struct Arc {
    std::string inputPin;
    std::string outputPin;

    bool operator==(const Arc& arc) const {
        return (inputPin == arc.inputPin && outputPin == arc.outputPin);
    }

};

template <>
struct std::hash<Arc> {
    std::size_t operator()(const Arc& arc) const noexcept {
        std::size_t hash = std::hash<std::string>{}(arc.inputPin);
        std::size_t h = std::hash<std::string>{}(arc.outputPin);
        boost::hash_combine(hash, h);
        return hash;
    }
};

struct LUT {
    std::vector<double> xIdxValues;
    std::vector<double> yIdxValues;
    std::vector<std::vector<double>> riseZMatrix;
    std::vector<std::vector<double>> fallZMatrix;
};

struct Cell {
    std::string name;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<std::string> bitFunctions;
    std::string capacitanceUnit;
    std::unordered_map<std::string, double> pinCapacitance;
    std::string timeUnit;
    std::unordered_map<Arc, LUT> delay;
    std::unordered_map<Arc, LUT> outputSlope;
    std::string internalPowerUnit;
    std::unordered_map<Arc, LUT> internalPower;
    std::string leakagePowerUnit;
    std::unordered_map<std::string, double> leakage;
};
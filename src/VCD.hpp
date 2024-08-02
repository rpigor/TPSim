#pragma once

#include "Simulator.hpp"
#include <boost/logic/tribool.hpp>
#include <unordered_map>
#include <set>
#include <string>
#include <iostream>

struct TriboolComp {
    bool operator()(boost::tribool tb1, boost::tribool tb2)  const {
        return tb1.value < tb2.value;
    }
};

class VCDBuffer {
private:

    unsigned long tick;
    std::unordered_map<std::string, std::set<boost::tribool, TriboolComp>> buffer;

public:

    VCDBuffer();
    void insert(const Transaction& t);
    void clear();
    void printVCD(std::ostream& os, const std::unordered_map<std::string, std::string>& wireIdMap) const;

};

class VCDFormatter {
private:

    std::ostream& os;
    std::unordered_map<std::string, std::string> wireIdMap;

public:

    VCDFormatter() = delete;
    VCDFormatter(std::ostream& os, const std::vector<std::string>& wires);
    void printHeader(const std::string& timescale) const;
    void printDefinitions(const std::string& moduleName) const;
    void printVarDumpInit() const;
    void printVarDumpBuffer(const VCDBuffer& buffer) const;

};
#pragma once

#include "Event.hpp"
#include <boost/logic/tribool.hpp>
#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <iostream>

std::string toString(boost::tribool);

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
    void insert(const Event& ev);
    void clear();
    void printVCD(std::ostream& os, const std::unordered_map<std::string, std::string>& wireIdMap) const;

};

class VCDFormatter {
private:

    std::unordered_map<std::string, std::string> wireIdMap;

public:

    VCDFormatter() = delete;
    VCDFormatter(const std::vector<std::string>& wires);
    void printHeader(std::ostream& os, const std::string& timescale) const;
    void printDefinitions(std::ostream& os, const std::string& moduleName) const;
    void printVarDumpInit(std::ostream& os) const;
    void printVarDumpBuffer(std::ostream& os, const VCDBuffer& buffer) const;

};
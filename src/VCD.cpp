#include "VCD.hpp"

VCDBuffer::VCDBuffer() : tick(-1) { }

void VCDBuffer::insert(const Transaction& t) {
    if (tick != t.tick) {
        buffer.clear();
        tick = t.tick;
    }
    buffer[t.wire].insert(t.value);
}

void VCDBuffer::clear() {
    buffer.clear();
    tick = -1;
}

void VCDBuffer::printVCD(std::ostream& os, const std::unordered_map<std::string, std::string>& wireIdMap) const {
    os << "#" << tick << "\n";
    for (auto& tupIt : buffer) {
        if (tupIt.second.size() == 1) {
            os << toString(*tupIt.second.begin()) << wireIdMap.at(tupIt.first) << "\n";
        }
        else {
            auto tbIt = std::find_if(tupIt.second.begin(), tupIt.second.end(), [](boost::tribool tb) { return !indeterminate(tb); });
            if (tbIt != tupIt.second.end()) {
                os << toString(*tbIt) << wireIdMap.at(tupIt.first) << "\n";
            }
            else {
                os << "x" << wireIdMap.at(tupIt.first) << "\n";
            }
        }
    }
}

VCDFormatter::VCDFormatter(std::ostream& os, const std::vector<std::string>& wires)
: os(os) {
    const char minWireChar = 33;
    const char maxWireChar = 126;
    unsigned char idx = 0;
    std::string wireId = std::string{minWireChar};
    for (auto& w : wires) {
        wireIdMap[w] = wireId;

        if (wireId[idx] < maxWireChar) {
            wireId[idx]++;
        }
        else {
            wireId.push_back(minWireChar);
            idx++;
        }
    }
}

void VCDFormatter::printHeader(const std::string& timescale) const {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    os  << "$version Simulator $end\n"
        << "$date " << std::put_time(&tm, "%d/%m/%Y %T") << " $end\n"
        << "$timescale 1" << timescale << " $end\n";
}

void VCDFormatter::printDefinitions(const std::string& moduleName) const {
    os << "$scope module " << moduleName << " $end\n";

    for (auto& t : wireIdMap) {
        os << "$var wire 1 " << t.second << " " << t.first << " $end\n";
    }

    os  << "$upscope $end\n"
        << "$enddefinitions $end\n";
}

void VCDFormatter::printVarDumpInit() const {
    os << "dumpvars\n";
}

void VCDFormatter::printVarDumpBuffer(const VCDBuffer& buffer) const {
    buffer.printVCD(os, wireIdMap);
}
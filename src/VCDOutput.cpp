#include "VCDOutput.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>

std::string toString(boost::tribool value) {
    if (indeterminate(value)) {
        return "x";
    } else if (value) {
        return "1";
    } else {
        return "0";
    }
}

VCDBuffer::VCDBuffer() : tick(-1) { }

void VCDBuffer::insert(const Event& ev) {
    if (tick != ev.tick) {
        buffer.clear();
        tick = ev.tick;
    }
    buffer[ev.wire].insert(ev.value);
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

VCDOutput::VCDOutput(const std::filesystem::path& outputFileName, std::ostream& stdOutStream, const std::vector<std::string>& wires)
: outputFileName(outputFileName), stdOutStream(stdOutStream) {
    outputFileStream.open(outputFileName);
    initWireIdMap(wires);
}

void VCDOutput::initWireIdMap(const std::vector<std::string>& wires) {
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

void VCDOutput::updateOnBegin(Simulation* sim) {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    outputFileStream  << "$version TPSim $end\n"
        << "$date " << std::put_time(&tm, "%d/%m/%Y %T") << " $end\n"
        << "$timescale 1" << sim->getConfiguration().timescale << " $end\n";

    outputFileStream << "$scope module " << sim->getModule().name << " $end\n";

    for (auto& t : wireIdMap) {
        outputFileStream << "$var wire 1 " << t.second << " " << t.first << " $end\n";
    }

    outputFileStream  << "$upscope $end\n"
        << "$enddefinitions $end\n";

    outputFileStream << "dumpvars\n";
}

void VCDOutput::updateAfterHandlingEvent(Simulation* sim, const Event& ev, unsigned long prevEvTime) {
    if (ev.tick != prevEvTime) {
        sameTickEvs.printVCD(outputFileStream, wireIdMap);
    }
    sameTickEvs.insert(ev);
}

void VCDOutput::updateOnEnd(Simulation* sim) {
    sameTickEvs.printVCD(outputFileStream, wireIdMap);
    stdOutStream << "[ INFO ] Saved VCD file to " << std::filesystem::canonical(outputFileName) << "." << std::endl;
}
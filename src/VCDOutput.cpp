#include "VCDOutput.hpp"
#include <iostream>

VCDOutput::VCDOutput(const std::filesystem::path& outputFileName, std::ostream& outputFileStream, std::ostream& os, const std::vector<std::string>& wires)
: toFile(true), outputFileName(outputFileName), outputFileStream(outputFileStream), os(os), vcd(wires) {

}

VCDOutput::VCDOutput(std::ostream& outputFileStream, std::ostream& os, const std::vector<std::string>& wires)
: toFile(false), outputFileStream(outputFileStream), os(os), vcd(wires) {

}

void VCDOutput::updateOnBegin(Simulation* sim) {
    vcd.printHeader(outputFileStream, sim->getConfiguration().timescale);
    vcd.printDefinitions(outputFileStream, sim->getModule().name);
    vcd.printVarDumpInit(outputFileStream);
}

void VCDOutput::updateAfterHandlingEvent(Simulation* sim, const Event& ev, unsigned long prevEvTime) {
    if (ev.tick != prevEvTime) {
        vcd.printVarDumpBuffer(outputFileStream, sameTickEvs);
    }
    sameTickEvs.insert(ev);
}

void VCDOutput::updateOnEnd(Simulation* sim) {
    vcd.printVarDumpBuffer(outputFileStream, sameTickEvs);
    if (toFile) {
        os << "[ INFO ] Saved VCD file to " << std::filesystem::canonical(outputFileName) << "." << std::endl;
    }
}
#include "StimuliParser.hpp"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <sstream>
#include <set>

StimuliParser::StimuliParser(const std::vector<std::string>& moduleInputs)
: moduleInputs(moduleInputs) {

}

void StimuliParser::read(const std::string& stimFile) {
    std::ifstream file(stimFile);
    std::string line;
    while (std::getline(file, line)) {
        std::size_t pos = line.find('=');
        if (pos == std::string::npos) {
            throw std::runtime_error("missing character '='");
        }

        std::vector<boost::tribool> inputStim;
        std::stringstream isLine(line.substr(pos+1));
        char c;
        while (isLine >> c) {
            if (c == ' ') {
                continue;
            }
            if (c == ';') {
                break;
            }
            if (c != '0' && c != '1') {
                throw std::runtime_error("illegal character '" + std::string{c} + "'");
            }
            inputStim.push_back(c == '0' ? false : true);
        }

        std::string pinName = line.substr(0, pos);
        boost::trim(pinName);
        stimuli[pinName] = inputStim;
    }

    std::set<std::string> moduleIn(moduleInputs.begin(), moduleInputs.end());
    std::set<std::string> inputs;
    for (auto& t : stimuli) {
        inputs.insert(t.first);
    }

    if (moduleIn != inputs) {
        throw std::runtime_error("stimuli input pin list does not match module input pin list");
    }
}

Stimuli StimuliParser::getStimuli() const {
    return stimuli;
}
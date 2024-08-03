#pragma once

#include <boost/logic/tribool.hpp>
#include <unordered_map>
#include <string>
#include <vector>

typedef std::unordered_map<std::string, std::vector<boost::tribool>> Stimuli;

class StimuliParser {
private:

    Stimuli stimuli;
    std::vector<std::string> moduleInputs;

public:

    StimuliParser(const std::vector<std::string>& moduleInputs);
    void read(const std::string& stimFile);
    Stimuli getStimuli() const;

};
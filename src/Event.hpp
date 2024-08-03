#pragma once

#include <boost/logic/tribool.hpp>
#include <string>

struct Event {
    std::string wire;
    double inputSlope;
    boost::tribool value;
    unsigned long tick;

    bool operator<(const Event& ev) const {
        return std::tie(tick, inputSlope) < std::tie(ev.tick, ev.inputSlope);
    }
};
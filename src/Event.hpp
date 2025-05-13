#pragma once

#include <boost/logic/tribool.hpp>
#include <ostream>
#include <string>

struct Event {
    std::string wire;
    double inputSlope;
    boost::tribool value;
    unsigned long tick;
    bool canceled;

    Event() {

    }

    Event(const std::string& wire, double inputSlope, boost::tribool value, unsigned long tick, bool canceled = false)
    : wire(wire), inputSlope(inputSlope), value(value), tick(tick), canceled(canceled) {

    }

    bool operator<(const Event& ev) const {
        return std::tie(tick, inputSlope) < std::tie(ev.tick, ev.inputSlope);
    }

    bool operator>(const Event& ev) const {
        return std::tie(tick, inputSlope) > std::tie(ev.tick, ev.inputSlope);
    }

    bool operator==(const Event& ev) const {
        return std::tie(wire, inputSlope, value, tick) == std::tie(ev.wire, ev.inputSlope, ev.value, ev.tick);
    }

    bool operator!=(const Event& ev) const {
        return std::tie(wire, inputSlope, value, tick) != std::tie(ev.wire, ev.inputSlope, ev.value, ev.tick);
    }

    friend std::ostream& operator<<(std::ostream& stream, const Event& ev) {
        return stream << "Event{" << ev.wire << ", " << ev.inputSlope << ", " << (boost::indeterminate(ev.value) ? "indeterminate" : (ev.value == true ? "true" : "false")) << ", " << ev.tick << "}";
    }
};
#pragma once

#include "Event.hpp"
#include <boost/logic/tribool.hpp>
#include <queue>
#include <vector>
#include <string>
#include <list>
#include <unordered_map>

class TimingWheel {
private:

    std::size_t criticalPathDelay;
    std::size_t currentTick;
    std::size_t currentIdx;

    std::vector<std::list<Event>> wheel;
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> queue;

public:

    TimingWheel(std::size_t criticalPathDelay, const std::vector<Event>& stimuli);
    TimingWheel(std::size_t criticalPathDelay, const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli, double stimuliSlope, unsigned long stimuliPeriod);

    void scheduleEvent(const Event& ev);
    std::list<Event> consumeNextEvents();
    unsigned long getCurrentEventTick() const;

};
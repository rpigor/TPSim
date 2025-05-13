#pragma once

#include "Event.hpp"
#include <boost/logic/tribool.hpp>
#include <queue>
#include <vector>
#include <string>
#include <list>
#include <unordered_map>
#include <memory>

struct CmpEventSharedPtr {
    bool operator()(std::shared_ptr<Event> lhs, std::shared_ptr<Event> rhs) const {
        return lhs->tick > rhs->tick;
    }
};

class TimingWheel {
private:

    std::size_t criticalPathDelay;
    std::size_t currentTick;
    std::size_t currentIdx;

    std::vector<std::list<std::shared_ptr<Event>>> wheel;
    std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, CmpEventSharedPtr> queue;
    std::unordered_map<std::string, std::shared_ptr<Event>> pendingEventPerNet;

public:

    TimingWheel(std::size_t criticalPathDelay, const std::vector<Event>& stimuli);
    TimingWheel(std::size_t criticalPathDelay, const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli, double stimuliSlope, unsigned long stimuliPeriod);

    void scheduleEvent(const Event& ev);
    std::list<Event> consumeNextEvents();
    unsigned long getCurrentEventTick() const;

};
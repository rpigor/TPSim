#pragma once

#include "Event.hpp"
#include <boost/logic/tribool.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>

class EventQueue {
private:

    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> queue;

public:

    EventQueue() = default;
    EventQueue(const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli, const std::vector<std::string>& inputNames, unsigned long clockPeriod, double inputSlope);
    void push(const Event& ev);
    void pop();
    Event top() const;
    bool empty() const;

};
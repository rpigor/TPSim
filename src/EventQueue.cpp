#include "EventQueue.hpp"
#include <algorithm>

EventQueue::EventQueue(const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli, const std::vector<std::string>& inputNames, unsigned long clockPeriod, double inputSlope) {
    unsigned long stimuliTime = 0;
    for (const auto& inputName : inputNames) {
        std::vector<boost::tribool> inputVector = stimuli.at(inputName);
        Event prevEvent{inputName, inputSlope, !inputVector[0], 0};
        for (auto tb : inputVector) {
            if (prevEvent.value == tb) {
                stimuliTime += clockPeriod;
                continue;
            }

            Event ev{inputName, inputSlope, tb, stimuliTime};
            push(ev);

            prevEvent = ev;
            stimuliTime += clockPeriod;
        }
        stimuliTime = 0;
    }
}

void EventQueue::push(const Event& ev) {
    queue.push(ev);
}

void EventQueue::pop() {
    queue.pop();
}

Event EventQueue::top() const {
    return queue.top();
}

bool EventQueue::empty() const {
    return queue.empty();
}
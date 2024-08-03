#include "EventQueue.hpp"
#include <algorithm>

EventQueue::EventQueue(const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli, const std::vector<std::string>& inputNames, unsigned long clockPeriod) {
    unsigned long stimuliTime = 0;
    for (const auto& inputName : inputNames) {
        std::vector<boost::tribool> inputVector = stimuli.at(inputName);
        Event prevEvent{inputName, 0.0, !inputVector[0], 0};
        for (auto tb : inputVector) {
            if (prevEvent.value == tb) {
                stimuliTime += clockPeriod;
                continue;
            }

            Event ev{inputName, 0.0, tb, stimuliTime};
            list.push_back(ev);

            prevEvent = ev;
            stimuliTime += clockPeriod;
        }
        stimuliTime = 0;
    }
}

void EventQueue::push(const Event& ev) {
    list.push_back(ev);
}

void EventQueue::pop() {
    auto it = std::min_element(list.begin(), list.end(), [](const Event& i, const Event& j) { return i < j; });
    Event t = *it;
    list.erase(it);
}

Event EventQueue::top() const {
    auto it = std::min_element(list.begin(), list.end(), [](const Event& i, const Event& j) { return i < j; });
    return *it;
}

bool EventQueue::empty() const {
    return list.empty();
}
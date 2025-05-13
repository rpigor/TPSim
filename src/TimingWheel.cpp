#include "TimingWheel.hpp"

TimingWheel::TimingWheel(std::size_t criticalPathDelay, const std::vector<Event>& stimuli)
: criticalPathDelay(criticalPathDelay), currentTick(0), currentIdx(0), wheel(criticalPathDelay, std::list<std::shared_ptr<Event>>{}) {
    if (criticalPathDelay == 0) {
        throw std::runtime_error("critical path delay cannot be zero.");
    }

    // initialize queue with stimuli
    for (auto& s : stimuli) {
        queue.push(std::make_shared<Event>(s));
    }
}

TimingWheel::TimingWheel(std::size_t criticalPathDelay, const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli, double stimuliSlope, unsigned long stimuliPeriod)
: criticalPathDelay(criticalPathDelay), currentTick(0), currentIdx(0), wheel(criticalPathDelay, std::list<std::shared_ptr<Event>>{}) {
    if (criticalPathDelay == 0) {
        throw std::runtime_error("critical path delay cannot be zero.");
    }

    unsigned long elapsedTick = 0;
    for (const auto& [wire, values] : stimuli) {
        elapsedTick = 0;
        boost::tribool prevValue = boost::indeterminate;
        for (const auto& v : values) {
            if (prevValue == v) { // do not create new event when there's no change in logic value
                elapsedTick += stimuliPeriod;
                continue;
            }

            queue.push(std::make_shared<Event>(wire, stimuliSlope, v, elapsedTick));

            elapsedTick += stimuliPeriod;
            prevValue = v;
        }
    }
}

void TimingWheel::scheduleEvent(const Event& ev) {
    std::size_t absoluteTick = currentTick + currentIdx;
    if (ev.tick < absoluteTick) { // out of wheel lower bound
        throw std::runtime_error("event tick < timing wheel current tick.");
    }

    // cancel previously schedule events happening in the same net
    // this maintains the functional correcteness of the simulation for events in the same net
    if (pendingEventPerNet.find(ev.wire) != pendingEventPerNet.end()) {

        // if the event occurs after a previously scheduled event, then abort the previous event
        if (pendingEventPerNet[ev.wire]->tick > ev.tick) {
            pendingEventPerNet[ev.wire]->canceled = true;
        }
    }

    auto evPtr = std::make_shared<Event>(ev);
    pendingEventPerNet[ev.wire] = evPtr; //

    if (ev.tick > (absoluteTick + criticalPathDelay - 1)) { // out of wheel upper bound, push to priority queue
        queue.push(evPtr);
        return;
    }

    std::size_t index;
    if (currentTick != 0) {
        index = ev.tick % currentTick;
    }
    else {
        index = ev.tick;
    }
    wheel.at(index).push_back(evPtr);
}

std::list<Event> TimingWheel::consumeNextEvents() {
    unsigned long emptyCount = 0;
    std::list<Event> events;
    while (events.empty()) {
        if (emptyCount >= (criticalPathDelay*2) && queue.empty()) {
            return events;
        }

        if (!wheel.at(currentIdx).empty()) { // pop events from the timing wheel
            for (const auto& evPtr : wheel.at(currentIdx)) {
                events.push_back(*evPtr);
            }
            wheel[currentIdx].clear();
        }
        while (!queue.empty() && (queue.top()->tick == (currentTick + currentIdx))) { // pop events from the priority queue
            events.push_back(*queue.top());
            queue.pop();
        }

        // move and reset the timing wheel
        currentIdx++;
        if ((currentIdx % criticalPathDelay) == 0) {
            currentTick += criticalPathDelay;
            currentIdx = 0;
        }

        emptyCount++;
    }

    return events;
}

unsigned long TimingWheel::getCurrentEventTick() const {
    return currentTick + currentIdx;
}
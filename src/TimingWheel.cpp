#include "TimingWheel.hpp"

TimingWheel::TimingWheel(std::size_t criticalPathDelay, const std::vector<Event>& stimuli)
: criticalPathDelay(criticalPathDelay), currentTick(0), currentIdx(0), wheel(criticalPathDelay, std::list<Event>{}), queue(stimuli.begin(), stimuli.end()) {
    if (criticalPathDelay == 0) {
        throw std::runtime_error("critical path delay cannot be zero.");
    }
}

TimingWheel::TimingWheel(std::size_t criticalPathDelay, const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli, double stimuliSlope, unsigned long stimuliPeriod)
: criticalPathDelay(criticalPathDelay), currentTick(0), currentIdx(0), wheel(criticalPathDelay, std::list<Event>{}) {
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

            Event ev{wire, stimuliSlope, v, elapsedTick};
            queue.push(ev);

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
    else if (ev.tick > (absoluteTick + criticalPathDelay - 1)) { // out of wheel upper bound, push to priority queue
        queue.push(ev);
        return;
    }

    std::size_t index;
    if (currentTick != 0) {
            index = ev.tick % currentTick;
    }
    else {
        index = ev.tick;
    }
    wheel.at(index).push_back(ev);
}

std::list<Event> TimingWheel::consumeNextEvents() {
    unsigned long emptyCount = 0;
    std::list<Event> events;
    while (events.empty()) {
        if (emptyCount >= (criticalPathDelay*2) && queue.empty()) {
            return events;
        }

        if (!wheel.at(currentIdx).empty()) { // pop events from the timing wheel
            events.insert(events.end(), wheel.at(currentIdx).begin(), wheel.at(currentIdx).end());
            wheel[currentIdx].clear();
        }
        while (!queue.empty() && (queue.top().tick == (currentTick + currentIdx))) { // pop events from the priority queue
            events.push_back(queue.top());
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
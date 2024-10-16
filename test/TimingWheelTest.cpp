#include <catch2/catch_test_macros.hpp>
#include "../src/TimingWheel.hpp"
#include <set>

TEST_CASE( "timing wheel works", "[wheel]" ) {

    std::vector<Event> stimuli{Event{"w1", 0.0012, true, 10}, Event{"w2", 0.0005, false, 12}, Event{"w1", 0.001, false, 15}};
    TimingWheel wheel(6, stimuli);
    wheel.scheduleEvent(Event{"w2", 0.0003, true, 3});
    wheel.scheduleEvent(Event{"w1", 0.0008, true, 5});
    wheel.scheduleEvent(Event{"w1", 0.0007, false, 8}); // long event
    wheel.scheduleEvent(Event{"w3", 0.0010, true, 10}); // long event

    auto evList = wheel.consumeNextEvents();
    std::set<Event> evSet(evList.begin(), evList.end());
    std::set<Event> correctEvSet{Event{"w2", 0.0003, true, 3}};
    REQUIRE(evSet == correctEvSet);
    REQUIRE(wheel.getCurrentEventTick() == 4);

    evList = wheel.consumeNextEvents();
    std::set<Event> evSet2(evList.begin(), evList.end());
    std::set<Event> correctEvSet2{Event{"w1", 0.0008, true, 5}};
    REQUIRE(evSet2 == correctEvSet2);
    REQUIRE(wheel.getCurrentEventTick() == 6);

    wheel.scheduleEvent(Event{"w2", 0.0005, false, 8});
    evList = wheel.consumeNextEvents();
    std::set<Event> evSet3(evList.begin(), evList.end());
    std::set<Event> correctEvSet3{Event{"w1", 0.0007, false, 8}, Event{"w2", 0.0005, false, 8}};
    REQUIRE(evSet3 == correctEvSet3);
    REQUIRE(wheel.getCurrentEventTick() == 9);

    evList = wheel.consumeNextEvents();
    std::set<Event> evSet4(evList.begin(), evList.end());
    std::set<Event> correctEvSet4{Event{"w3", 0.0010, true, 10}, Event{"w1", 0.0012, true, 10}};
    REQUIRE(evSet4 == correctEvSet4);
    REQUIRE(wheel.getCurrentEventTick() == 11);

    evList = wheel.consumeNextEvents();
    std::set<Event> evSet5(evList.begin(), evList.end());
    std::set<Event> correctEvSet5{Event{"w2", 0.0005, false, 12}};
    REQUIRE(evSet5 == correctEvSet5);

    evList = wheel.consumeNextEvents();
    std::set<Event> evSet6(evList.begin(), evList.end());
    std::set<Event> correctEvSet6{Event{"w1", 0.001, false, 15}};
    REQUIRE(evSet6 == correctEvSet6);

    evList = wheel.consumeNextEvents();
    REQUIRE(evList.empty());
}
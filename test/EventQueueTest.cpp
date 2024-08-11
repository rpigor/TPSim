#include <catch2/catch_test_macros.hpp>
#include "../src/EventQueue.hpp"

TEST_CASE( "event queue works", "[queue]" ) {

    EventQueue q;
    REQUIRE(q.empty());
    Event ev1{"w1", 0.0012, true, 10};
    Event ev2{"w2", 0.0005, false, 12};
    q.push(ev1);
    REQUIRE(!q.empty());
    REQUIRE(q.top() == ev1);
    q.push(ev2);
    REQUIRE(q.top() == ev1);

    SECTION( "push works" ) {
        Event ev3{"w3", 0.0023, true, 8};
        q.push(ev3);
        REQUIRE(q.top() == ev3);

        q.push(Event{"w4", 0.0021, false, 15});
        REQUIRE(q.top() == ev3);
    }

    SECTION( "pop works" ) {
        q.pop();
        REQUIRE(q.top() == ev2);

        Event ev3{"w3", 0.0023, true, 8};
        q.push(ev3);
        q.pop();
        REQUIRE(q.top() == ev2);

        q.pop();
        REQUIRE(q.empty());
    }

}
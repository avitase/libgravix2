#include "api.h"
#include <catch2/catch.hpp>
#include <cmath>

TEST_CASE("Test planet", "[planet]")
{
    auto planets = grvx_new_planets(3);
    REQUIRE(grvx_count_planets(planets) == 3);

    int rc = 0;

    rc = grvx_set_planet(planets, 0, .1, .2);
    REQUIRE(rc == 0);

    rc = grvx_set_planet(planets, 1, .3, .4);
    REQUIRE(rc == 0);

    rc = grvx_set_planet(planets, 2, .5, .6);
    REQUIRE(rc == 0);

    double lat, lon;

    rc = grvx_get_planet(planets, 0, &lat, &lon);
    REQUIRE(rc == 0);
    REQUIRE(lat == Approx(.1));
    REQUIRE(lon == Approx(.2));

    rc = grvx_get_planet(planets, 1, &lat, &lon);
    REQUIRE(rc == 0);
    REQUIRE(lat == Approx(.3));
    REQUIRE(lon == Approx(.4));

    rc = grvx_get_planet(planets, 2, &lat, &lon);
    REQUIRE(rc == 0);
    REQUIRE(lat == Approx(.5));
    REQUIRE(lon == Approx(.6));

    unsigned n = grvx_pop_planet(planets);
    REQUIRE(n == 2U);

    rc = grvx_get_planet(planets, 0, &lat, &lon);
    REQUIRE(rc == 0);
    REQUIRE(lat == Approx(.1));
    REQUIRE(lon == Approx(.2));

    rc = grvx_get_planet(planets, 1, &lat, &lon);
    REQUIRE(rc == 0);
    REQUIRE(lat == Approx(.3));
    REQUIRE(lon == Approx(.4));

    grvx_delete_planets(planets);
}

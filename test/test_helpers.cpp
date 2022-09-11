#include "api.h"
#include <catch2/catch.hpp>
#include <cmath>

TEST_CASE("Test lat/lon conversions", "[helpers]")
{
    REQUIRE(grvx_lat(.3) == Approx(std::asin(.3)));
    REQUIRE(grvx_lon(.2, .4) == Approx(std::atan2(.2, .4)));

    double sa = std::sin(.2);
    double sb = std::sin(.3);
    double ca = std::cos(.2);
    double cb = std::cos(.3);
    double vx = -.1 * sa * sb + .4 * ca * cb;
    double vy = -.1 * sa * cb - .4 * ca * sb;
    double vz = +.1 * ca;
    REQUIRE(grvx_vlat(vx, vy, vz, .2, .3) == Approx(vz / ca));
    REQUIRE(grvx_vlon(vx, vy, vz, .3) == Approx(vx * cb - vy * sb));
}
#include "libgravix2/api.h"
#include <catch2/catch.hpp>
#include <numbers>

TEST_CASE("Test orbital time", "[integrator]")
{
    const double H = 1e-3;

    const double V = grvx_v_esc();
    auto [v, psi] =
        GENERATE_COPY(std::make_pair(.5 * V, 0.),                       //
                      std::make_pair(.99 * V, std::numbers::pi),        //
                      std::make_pair(1.01 * V, -std::numbers::pi / 4.), //
                      std::make_pair(2. * V, 1337.));                   //

    auto p = grvx_new_planets(1);
    REQUIRE(grvx_count_planets(p) == 1);

    int rc = grvx_set_planet(p, 0, 0., 0.);
    REQUIRE(rc == 0);

    auto missiles = grvx_new_missiles(1);
    auto *m = grvx_get_trajectory(missiles, 0);
    rc = grvx_launch_missile(m, p, 0, v, psi);
    REQUIRE(rc == 0);

    int premature = 0;
    unsigned n = 0;
    while (premature == 0) {
        n += grvx_propagate_missile(m, p, H, &premature);
    }

    auto n_exp = grvx_orb_period(v, H);
    REQUIRE(std::abs(n - n_exp) < 1);

    grvx_delete_missiles(missiles);
    grvx_delete_planets(p);
}
#include "libgravix2/api.h"
#include <catch2/catch.hpp>
#include <cmath>

TEST_CASE("Test missile", "[missile]")
{
    const double H = 1e-3;

    const double V = grvx_v_esc();
    REQUIRE(!std::isnan(V));
    REQUIRE(V > 0.);

    const double T = grvx_orb_period(2. * V, H);
    REQUIRE(!std::isnan(T));
    REQUIRE(T > 0.);

    auto planets = grvx_new_planets(1);
    REQUIRE(grvx_count_planets(planets) == 1);

    int rc = grvx_set_planet(planets, 0, 0., 0.);
    REQUIRE(rc == 0);

    auto missiles = grvx_new_missiles(1);
    auto *m = grvx_get_trajectory(missiles, 0);

    grvx_launch_missile(m, planets, 0, 2. * V, 0.);
    unsigned n_acc = 0;
    int premature = 0;
    while (premature == 0) {
        auto n = grvx_propagate_missile(m, planets, H, &premature);
        n_acc += n;

        for (unsigned i = 0; i < n; i++) {
            double qx = m->x[i][0];
            double qy = m->x[i][1];
            double qz = m->x[i][2];
            double px = m->v[i][0];
            double py = m->v[i][1];
            double pz = m->v[i][2];
            INFO("Loop iteration i=" << i)
            REQUIRE(qx * qx + qy * qy + qz * qz == Approx(1.));
            REQUIRE(qx * px + qy * py + qz * pz == Approx(0.).margin(1e-15));
        }
    }
    REQUIRE(n_acc == ((unsigned)T + 1));

    auto *cfg = grvx_get_config();
    const auto trj_size = cfg->trajectory_size;
    grvx_free_config(cfg);

    grvx_launch_missile(m, planets, 0, V, -1.5);
    premature = 0;
    const int N = (int)(30. * T / trj_size);
    for (int i = 0; i < N && premature == 0; i++) {
        grvx_propagate_missile(m, planets, H, &premature);
    }
    REQUIRE(premature == 0);

    grvx_delete_missiles(missiles);
    grvx_delete_planets(planets);
}

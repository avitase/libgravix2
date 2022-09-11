#include "api.h"
#include <catch2/catch.hpp>
#include <cmath>
#include <numbers>

static double dist(double xyz[3])
{
    return std::acos(std::cos(grvx_lat(xyz[2])) *
                     std::cos(grvx_lon(xyz[0], xyz[1])));
}

TEST_CASE("Test small circle dynamics")
{
    const int N = 100;
    const double H = 1e-3;

    auto r = 10. / 180. * std::numbers::pi;
    auto v = grvx_v_scrcl(r);

    auto p = grvx_new_planets(1);
    auto rc = grvx_set_planet(p, 0, 0., 0.);
    REQUIRE(rc == 0);

    auto trj = grvx_new_missiles(1);
    auto *m = grvx_get_trajectory(trj, 0);
    rc = grvx_init_missile(m, r, 0., v, 0., 1.);
    REQUIRE(rc == 0);

    int premature = 0;
    for (int i = 0; i < N; i++) {
        auto n = grvx_propagate_missile(m, p, H, &premature);
        REQUIRE(premature == 0);

        for (int j = 0; j < n; j++) {
            const double dr = dist(m->x[j]) - r;

            double vx = m->v[j][0];
            double vy = m->v[j][1];
            double vz = m->v[j][2];
            double dv = std::sqrt(vx * vx + vy * vy + vz * vz) - v;
            REQUIRE(dr == Approx(0.).margin(1e-10));
            REQUIRE(dv == Approx(0.).margin(1e-10));
        }
    }

    grvx_delete_missiles(trj);
    grvx_delete_planets(p);
}

#include "api.h"
#include <catch2/catch.hpp>
#include <cmath>
#include <numbers>

TEST_CASE("Test basic integrator properties for single planets", "[integrator]")
{
    auto planets = grvx_new_planets(1);
    REQUIRE(grvx_count_planets(planets) == 1);

    int rc = grvx_set_planet(planets, 0, 0., 0.);
    REQUIRE(rc == 0);

    auto missiles = grvx_new_missiles(1);
    auto *m = grvx_get_trajectory(missiles, 0);

    rc = grvx_launch_missile(m, planets, 0, 0., 0.);
    REQUIRE(rc == 0);

    auto *cfg = grvx_get_config();
    const auto N = cfg->trajectory_size;
    grvx_free_config(cfg);

    REQUIRE(m->x[0][0] == Approx(m->x[N - 1][0]));
    REQUIRE(m->x[0][1] == Approx(m->x[N - 1][1]));
    REQUIRE(m->x[0][2] == Approx(m->x[N - 1][2]));

    REQUIRE(m->v[0][0] == Approx(m->v[N - 1][0]));
    REQUIRE(m->v[0][1] == Approx(m->v[N - 1][1]));
    REQUIRE(m->v[0][2] == Approx(m->v[N - 1][2]));

    int premature;
    auto n = grvx_propagate_missile(m, planets, 1e-4, &premature);
    REQUIRE(n > 0);
    REQUIRE(premature == 1);

    grvx_delete_missiles(missiles);
    grvx_delete_planets(planets);
}

TEST_CASE("Test integrator properties for multiple planets", "[integrator]")
{
    const double H = 1e-3;

    auto uni2 = grvx_new_planets(2);
    auto uni3 = grvx_new_planets(3);
    REQUIRE(grvx_count_planets(uni2) == 2);
    REQUIRE(grvx_count_planets(uni3) == 3);

    double lat1 = 0.;
    double lat2 = 1.;
    double lat3 = 2.;

    int rc = 0;
    rc = grvx_set_planet(uni2, 0, lat1, /*lon=*/0);
    REQUIRE(rc == 0);
    rc = grvx_set_planet(uni2, 1, lat2, /*lon=*/0);
    REQUIRE(rc == 0);
    rc = grvx_set_planet(uni3, 0, lat1, /*lon=*/0);
    REQUIRE(rc == 0);
    rc = grvx_set_planet(uni3, 1, lat2, /*lon=*/0);
    REQUIRE(rc == 0);
    rc = grvx_set_planet(uni3, 2, lat3, /*lon=*/0);
    REQUIRE(rc == 0);

    auto missiles = grvx_new_missiles(3);
    auto *m1 = grvx_get_trajectory(missiles, 0);
    auto *m2 = grvx_get_trajectory(missiles, 1);
    auto *m3 = grvx_get_trajectory(missiles, 2);

    double lat = 0.;
    double lon = std::numbers::pi;
    double v = 0;
    double dlat = 1.0;
    double dlon = 0.0;
    grvx_init_missile(m1, lat, lon, v, dlat, dlon);
    grvx_init_missile(m2, lat, lon, v, dlat, dlon);
    grvx_init_missile(m3, lat, lon, v, dlat, dlon);

    int premature;

    // 2 planets
    auto n1 = grvx_propagate_missile(m1, uni2, H, &premature);
    REQUIRE(n1 > 10);

    // 3 planets
    auto n2 = grvx_propagate_missile(m2, uni3, H, &premature);
    REQUIRE(n2 > 10);

    auto n_planets = grvx_pop_planet(uni3);
    REQUIRE(n_planets == 2);

    // 2 planets
    auto n3 = grvx_propagate_missile(m3, uni3, H, &premature);
    REQUIRE(n3 > 10);

    auto n = std::min(n1, n2);
    REQUIRE(n > 10);
    REQUIRE(m1->x[n - 1][0] != Approx(m2->x[n - 1][0]));
    REQUIRE(m1->x[n - 1][1] != Approx(m2->x[n - 1][1]));
    REQUIRE(m1->x[n - 1][2] != Approx(m2->x[n - 1][2]));
    REQUIRE(m1->v[n - 1][0] != Approx(m2->v[n - 1][0]));
    REQUIRE(m1->v[n - 1][1] != Approx(m2->v[n - 1][1]));
    REQUIRE(m1->v[n - 1][2] != Approx(m2->v[n - 1][2]));

    REQUIRE(n1 == n3);
    for (int i = 0; i < n1; i++) {
        REQUIRE(m1->x[i][0] == Approx(m3->x[i][0]));
        REQUIRE(m1->x[i][1] == Approx(m3->x[i][1]));
        REQUIRE(m1->x[i][2] == Approx(m3->x[i][2]));
        REQUIRE(m1->v[i][0] == Approx(m3->v[i][0]));
        REQUIRE(m1->v[i][1] == Approx(m3->v[i][1]));
        REQUIRE(m1->v[i][2] == Approx(m3->v[i][2]));
    }

    grvx_delete_missiles(missiles);
    grvx_delete_planets(uni2);
    grvx_delete_planets(uni3);
}

TEST_CASE("Test symmetry of integrator", "[integrator]")
{
    auto *cfg = grvx_get_config();
    const auto MAX_TRJ_SIZE = cfg->trajectory_size;
    grvx_free_config(cfg);

    const unsigned N = 50;
    REQUIRE(N < MAX_TRJ_SIZE);

    const double H = 1e-3;
    const double V0 = 2 * grvx_v_esc();
    REQUIRE(N < grvx_orb_period(V0, H));

    auto p = grvx_new_planets(1);
    REQUIRE(grvx_count_planets(p) == 1);

    int rc = grvx_set_planet(p, 0, 0., 0.);
    REQUIRE(rc == 0);

    auto trj = grvx_new_missiles(2);

    auto *m1 = grvx_get_trajectory(trj, 0);
    auto *m2 = grvx_get_trajectory(trj, 1);

    rc = grvx_launch_missile(m1, p, 0, V0, -std::numbers::pi / 2);
    REQUIRE(rc == 0);

    int premature;
    auto n = grvx_propagate_missile(m1, p, H, &premature);
    REQUIRE(n >= N);

    /*
     *  \
     *  o  x  x  x  x  x  x  x
     *  /  0  1  2  3  4  5  6
     *  ^-- planet     ^-- N = 5
     */
    double *x = m1->x[N - 1];
    double *v = m1->v[N - 1];

    double lat2 = grvx_lat(x[2]);
    double lon2 = grvx_lon(x[0], x[1]);
    double vlat2 = grvx_vlat(v[0], v[1], v[2], lat2, lon2);
    double vlon2 = grvx_vlon(v[0], v[1], v[2], lon2);
    double v2_mag = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    rc = grvx_init_missile(m2, lat2, lon2, v2_mag, -vlat2, -vlon2);
    REQUIRE(rc == 0);

    auto n2 = grvx_propagate_missile(m2, p, H, &premature);
    REQUIRE(premature != 0);

    // integration stops *after* the missile has passed the rim
    REQUIRE((n2 == N || n2 == N + 1));

    for (unsigned i = 0; i < N - 2; i++) {
        INFO("Iteration i=" << i);

        unsigned j = N - i - 2; // init is not part of trajectory
        REQUIRE(m1->x[j][0] == Approx(m2->x[i][0]));
        REQUIRE(m1->x[j][1] == Approx(m2->x[i][1]));
        REQUIRE(m1->x[j][2] == Approx(m2->x[i][2]));
        REQUIRE(m1->v[j][0] == Approx(-m2->v[i][0]));
        REQUIRE(m1->v[j][1] == Approx(-m2->v[i][1]));
        REQUIRE(m1->v[j][2] == Approx(-m2->v[i][2]));
    }

    grvx_delete_missiles(trj);
    grvx_delete_planets(p);
}

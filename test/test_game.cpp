#include "helpers.hpp"
#include "libgravix2/game.h"
#include <catch2/catch.hpp>
#include <chrono>
#include <cmath>
#include <future>
#include <limits>
#include <numbers>
#include <random>
#include <set>

static void
test_init_planets(GrvxPlanets *planets, double min_dist, unsigned seed)
{
    auto old_seed = seed;
    auto n = std::async(
        std::launch::async, *grvx_rnd_init_planets, planets, &seed, min_dist);

    using namespace std::chrono_literals;
    auto status = n.wait_for(50ms);
    if (status != std::future_status::ready) {
        auto n_planets = grvx_count_planets(planets);
        FAIL("Initializing failed: " << n_planets << " planets, seed=" << seed
                                     << ", min_dist=" << min_dist);
    }

    REQUIRE(n.get() < 100);
    REQUIRE(old_seed != seed);

    const auto n_planets = grvx_count_planets(planets);
    for (unsigned i = 0; i < n_planets; i++) {
        double lat1, lon1;
        grvx_get_planet(planets, i, &lat1, &lon1);
        for (unsigned j = 0; j < n_planets; j++) {
            if (i != j) {
                double lat2, lon2;
                grvx_get_planet(planets, j, &lat2, &lon2);
                REQUIRE(grvx::testing::great_circle_distance(
                            lat1, lon1, lat2, lon2) > min_dist);
            }
        }
    }
}

TEST_CASE("Test planet initialization", "[game]")
{
    const double min_dist = .1;
    unsigned seed = GENERATE(0U, 1U, 2U, 3U, 42U, 1337U, 1200458331U);
    unsigned n_planets = GENERATE(1U, 2U, 3U, 4U, 5U);

    auto *planets = grvx_new_planets(n_planets);
    test_init_planets(planets, min_dist, seed);
    grvx_delete_planets(planets);
}

TEST_CASE("Test planet distribution", "[game]")
{
    const double min_dist = .1;

    using seed_type = std::uint32_t;
    std::set<seed_type> seed_blacklist;

    std::mt19937 rnd{42};
    std::uniform_int_distribution<seed_type> seed_distribution(
        0, std::numeric_limits<seed_type>::max());
    std::uniform_int_distribution<seed_type> planet_distribution(2, 5);

    const unsigned N = 8000;
    std::vector<double> lat(N, 0.), lon(N, 0.);

    auto n_total = 0U;
    while (n_total < N) {
        seed_type seed;
        do {
            seed = seed_distribution(rnd);
        } while (seed_blacklist.contains(seed));
        seed_blacklist.insert(seed);

        auto n_planets = std::min(planet_distribution(rnd), N - n_total);
        auto *planets = grvx_new_planets(n_planets);
        test_init_planets(planets, min_dist, seed);
        for (auto i = 0U; i < n_planets; i++) {
            grvx_get_planet(planets, i, &lat[i + n_total], &lon[i + n_total]);
        }

        grvx_delete_planets(planets);

        n_total += n_planets;
    }
    REQUIRE(n_total == N);

    const double PI = std::numbers::pi;
    const std::vector lat_bins{-.5 * PI,
                               std::asin(-.75),
                               std::asin(-.50),
                               std::asin(-.25),
                               0.,
                               std::asin(+.25),
                               std::asin(+.50),
                               std::asin(+.75),
                               PI / 2.};
    const std::vector lon_bins{-1.00 * PI,
                               -0.75 * PI,
                               -0.50 * PI,
                               -0.25 * PI,
                               +0.00 * PI,
                               +0.25 * PI,
                               +0.50 * PI,
                               +0.75 * PI,
                               +1.00 * PI};

    std::vector lat_hist(lat_bins.size() - 1, 0);
    std::vector lon_hist(lon_bins.size() - 1, 0);

    auto fill_hist = [](auto &hist, const auto &bins, double value) {
        auto it = std::lower_bound(bins.begin(), bins.end(), value);
        auto idx = std::distance(bins.begin(), it) - 1;
        REQUIRE(idx >= 0);
        REQUIRE(idx < hist.size());
        hist[idx] += 1;
    };

    for (auto i = 0U; i < N; i++) {
        INFO("Try to fill latitude:" << lat[i]);
        fill_hist(lat_hist, lat_bins, lat[i]);

        INFO("Try to fill longitude:" << lon[i]);
        fill_hist(lon_hist, lon_bins, lon[i]);
    }

    REQUIRE(N / 8 == 1000);
    REQUIRE(lat_hist.size() == 8);
    REQUIRE(lon_hist.size() == 8);
    const int threshold = 1100; // probability < 0.001 (Poisson distribution)

    for (auto i = 0U; i < 8; i++) {
        REQUIRE(lat_hist[i] < threshold);
        REQUIRE(lon_hist[i] < threshold);
    }
}

TEST_CASE("Test tick")
{
    const double H = .1;

    auto planets = grvx_new_planets(2);
    grvx_set_planet(planets, 0, -std::numbers::pi / 2., 0.);
    grvx_set_planet(planets, 1, +std::numbers::pi / 2., 0.);

    auto game = grvx_init_game(planets);

    GrvxMissileLaunch m1{.t_start = 2.1, // start: 2.1
                         .dt_ping = 1.4, //  ping: 3.5
                         .dt_end = 2.0,  //   end: 4.1
                         .v = 1.,
                         .psi = 0.};
    GrvxMissileLaunch m2{.t_start = 1.5, // start:   1.5
                         .dt_ping = 1.6, //  ping:   3.1
                         .dt_end = 98.5, //   end: 100.0  (too late)
                         .v = 1.,
                         .psi = 0.};
    REQUIRE(grvx_request_launch(game, 0, &m1, H) == 0);
    REQUIRE(grvx_request_launch(game, 0, &m2, H) == 0);

    std::uint32_t t = 0;
    GrvxMissileObservation *obs = grvx_observe_or_tick(game, &t);
    REQUIRE(t == 1);
    REQUIRE(obs == nullptr);

    obs = grvx_observe_or_tick(game, &t);
    REQUIRE(t == 2);
    REQUIRE(obs == nullptr);

    obs = grvx_observe_or_tick(game, &t);
    REQUIRE(t == 3);
    REQUIRE(obs == nullptr);

    obs = grvx_observe_or_tick(game, &t);
    REQUIRE(t == 3);
    REQUIRE(obs != nullptr);
    REQUIRE(obs->t == Approx(3.1));
    REQUIRE(obs->planet_id > 1);

    obs = grvx_observe_or_tick(game, &t);
    REQUIRE(t == 3);
    REQUIRE(obs != nullptr);
    REQUIRE(obs->t == Approx(3.5));
    REQUIRE(obs->planet_id > 1);

    obs = grvx_observe_or_tick(game, &t);
    REQUIRE(t == 4);
    REQUIRE(obs == nullptr);

    while ((obs = grvx_observe_or_tick(game, &t)) == nullptr and t <= 100) {
    }
    REQUIRE(obs != nullptr);
    REQUIRE(obs->t < 100.);
    REQUIRE(obs->planet_id == 1);

    grvx_delete_game(game);
    grvx_delete_planets(planets);
}

#include "libgravix2/api.h"
#include "libgravix2/config.h"
#include <catch2/catch.hpp>
#include <string>

TEST_CASE("Test version", "[config]")
{
    REQUIRE_THAT(grvx_version(), Catch::Equals(GRVX_VERSION));
}

TEST_CASE("GRVX_POT_TYPE is either 2D or 3D", "[!shouldfail]")
{
    REQUIRE(GRVX_POT_TYPE != GRVX_POT_TYPE_2D);
    REQUIRE(GRVX_POT_TYPE != GRVX_POT_TYPE_3D);
}

TEST_CASE("Test config", "[config]")
{
    auto *cfg = grvx_get_config();

    SECTION("Potential type and approximation order")
    {
#if GRVX_POT_TYPE == GRVX_POT_TYPE_2D
        REQUIRE_THAT(cfg->pot_type, Catch::Equals("2D"));
#else // GRVX_POT_TYPE_3D
        REQUIRE_THAT(cfg->pot_type, Catch::Equals("3D"));
        REQUIRE(cfg->n_pot == GRVX_N_POT);
#endif
    }

    SECTION("Trajectory size")
    {
        REQUIRE(cfg->trajectory_size == GRVX_TRAJECTORY_SIZE);
    }

    SECTION("Integration steps")
    {
        REQUIRE(cfg->int_steps == GRVX_INT_STEPS);
    }

    SECTION("Minimal distance")
    {
        REQUIRE(cfg->min_dist == Approx(GRVX_MIN_DIST));
    }

    SECTION("Composition")
    {
        REQUIRE_THAT(cfg->composition_scheme,
                     Catch::Equals(GRVX_COMPOSITION_SCHEME));
        REQUIRE(cfg->n_stages == GRVX_COMPOSITION_STAGES);

        auto comp = std::string(cfg->composition_scheme);
        auto n_stages_exp = std::string("s") + std::to_string(cfg->n_stages);
        REQUIRE(comp.size() > n_stages_exp.size());

        auto n_stages = comp.substr(comp.size() - n_stages_exp.size());
        REQUIRE(n_stages == n_stages_exp);
    }

    grvx_free_config(cfg);
}
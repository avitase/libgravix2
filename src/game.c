#include "libgravix2/game.h"
#include "libgravix2/config.h"
#include "libgravix2/constants.h"
#include "libgravix2/observations.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

static double linear_congruential_engine(unsigned *state)
{
    *state = *state == 0 ? 1 : *state;

    // as recommended by Park, Miller, and Stockmeyer (1993)
    const unsigned a = 48271;
    const unsigned c = 0;
    const unsigned m = 2147483647;

    *state = (a * *state + c) % m;
    return (double)*state / (double)m;
}

static double
great_circle_distance(double lat1, double lat2, double lon1, double lon2)
{
    double s1 = sin(lat1);
    double s2 = sin(lat2);
    double c1 = cos(lat1);
    double c2 = cos(lat2);
    double d = cos(lon1 - lon2);
    return acos(s1 * s2 + c1 * c2 * d);
}

static unsigned
get_closest_planet(GrvxPlanetsHandle planets, double lat, double lon)
{
    double min_dist = 999.; // much larger than pi
    unsigned planet = 0;

    const unsigned n = grvx_count_planets(planets);
    for (unsigned i = 0; i < n; i++) {
        double p_lat;
        double p_lon;
        if (grvx_get_planet(planets, i, &p_lat, &p_lon) == 0) {
            double dist = great_circle_distance(lat, p_lat, lon, p_lon);
            if (dist < min_dist) {
                min_dist = dist;
                planet = i;
            }
        }
    }

    return planet;
}

static bool validate_launch(struct GrvxMissileLaunch *missile, double t)
{
    return (t <= missile->t_start) &&              //
           (missile->t_start < missile->t_end) &&  //
           (missile->t_start < missile->t_ping) && //
           (missile->t_ping <= missile->t_end);
}

static void
get_position(struct GrvxTrajectory *trj, double t_rel, double *lat, double *lon)
{
    unsigned idx = (unsigned)(t_rel * GRVX_TRAJECTORY_SIZE);
    assert(idx < GRVX_TRAJECTORY_SIZE);

    double x = trj->x[idx][0];
    double y = trj->x[idx][1];
    double z = trj->x[idx][2];

    *lat = grvx_lat(z);
    *lon = grvx_lon(x, y);
}

int grvx_rnd_init_planets(GrvxPlanetsHandle planets,
                          unsigned *seed,
                          double min_dist)
{
    const unsigned n_planets = grvx_count_planets(planets);

    double all_lats[n_planets];
    double all_lons[n_planets];

    int counter = 0;
    for (unsigned i = 0; i < n_planets; i++) {
        double lat;
        double lon;

        bool done;
        do {
            lat = asin(2. * linear_congruential_engine(seed) - 1.);
            lon = M_PI * (2. * linear_congruential_engine(seed) - 1.);
            counter += 1;

            done = true;
            for (unsigned j = 0; j < i && done; j++) {
                double lat2 = all_lats[j];
                double lon2 = all_lons[j];
                double dist = great_circle_distance(lat, lat2, lon, lon2);

                done &= (dist >= min_dist);
            }

        } while (!done);

        all_lats[i] = lat;
        all_lons[i] = lon;
        grvx_set_planet(planets, i, lat, lon);
    }

    return counter;
}

struct GrvxGame {
    unsigned tick;
    GrvxPlanetsHandle planets;
    GrvxTrajectoryBatch missiles;
    double v0;
    struct GrvxMissileObservation *observation;
    struct GrvxMissileObservations *observations;
};

GrvxGameHandle grvx_init_game(GrvxPlanetsHandle planets)
{
    struct GrvxGame *game = malloc(sizeof(struct GrvxGame));

    game->tick = 0U;
    game->planets = planets;
    game->missiles = grvx_new_missiles(1);
    game->v0 = grvx_v_esc();
    game->observation = 0;
    game->observations = 0;

    return game;
}

void grvx_delete_game(GrvxGameHandle game)
{
    grvx_delete_missiles(game->missiles);

    delete_observations(game->observations);
    free(game->observation);

    free(game);
}

int grvx_request_launch(GrvxGameHandle game,
                        unsigned planet_id,
                        struct GrvxMissileLaunch *missile,
                        double dt)
{
    double t = (double)game->tick;
    if (!validate_launch(missile, t)) {
        return 1;
    }

    struct GrvxTrajectory *trj = grvx_get_trajectory(game->missiles, 0);
    int rc = grvx_launch_missile(trj, game->planets, planet_id,
                                 missile->v / game->v0, missile->psi);
    if (rc != 0) {
        return rc;
    }

    const double trj_size = (double)GRVX_TRAJECTORY_SIZE;
    const double h = dt / (double)GRVX_INT_STEPS / trj_size;

    int premature = 0;
    while (premature != 1 && t < missile->t_end) {
        unsigned n = grvx_propagate_missile(trj, game->planets, h, &premature);

        double t_ping = -1.;
        double t_detonation = -1.;

        if (premature == 1) {
            double frac = (double)n / trj_size;

            double diff = missile->t_ping - t;
            if (0. <= diff && diff < frac) {
                t_ping = diff;
            }

            diff = missile->t_end - t;
            if (frac <= diff) {
                t_detonation = frac;
            }
        } else {
            double diff = missile->t_ping - t;
            if (0. <= diff && diff < 1.) {
                t_ping = diff;
            }
        }

        if (t_ping >= 0.) {
            struct GrvxMissileObservation *obs =
                malloc(sizeof(struct GrvxMissileObservation));
            obs->planet_id = grvx_count_planets(game->planets);
            obs->t = missile->t_ping;
            get_position(trj, t_ping, &obs->lat, &obs->lon);

            game->observations = add_observation(game->observations, obs);
        }

        if (t_detonation >= 0.) {
            double lat, lon;
            get_position(trj, t_ping, &lat, &lon);

            struct GrvxMissileObservation *obs =
                malloc(sizeof(struct GrvxMissileObservation));
            obs->planet_id = get_closest_planet(game->planets, lat, lon);
            obs->t = missile->t_ping;
            grvx_get_planet(game->planets, obs->planet_id, &obs->lat,
                            &obs->lon);

            game->observations = add_observation(game->observations, obs);
        }

        t += 1.;
    }

    return 0;
}

struct GrvxMissileObservation *grvx_observe_or_tick(GrvxGameHandle game,
                                                    unsigned *t)
{
    struct GrvxMissileObservation *obs = 0;

    if (game->observations == 0 || game->observations->obs->t > game->tick) {
        game->tick += 1;
    } else {
        free(game->observation);
        game->observations =
            pop_observation(game->observations, &game->observation);

        obs = game->observation;
    }

    *t = game->tick;

    return obs;
}

/*!
 * \file game.h
 * \brief API of the Gravix2 Game
 *
 * See module \ref Game for a detailed description.
 */

/*!
 * \defgroup Game Game
 * \brief API for creating and interacting with games.
 *
 * @{
 *
 * TODO: explain relation to libgravix2 (extension of its API)
 * TODO: explain (fractional) ticks
 * TODO: unit is radians
 */

#pragma once

#include "libgravix2/api.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct GrvxGame;

/*!
 * \brief Handle to a game.
 *
 * Games are intentionally opaque in the API and should only be referred to by
 * their respective handle. Internals are subjects to change.
 */
typedef struct GrvxGame *GrvxGameHandle;

/*!
 * \brief Settings of a scheduled missile launch.
 *
 * Launch configuration to be passed to grvx_request_launch().
 */
struct GrvxMissileLaunch {
    /*!
     * \brief Launch.
     *
     * Requested launch time.
     */
    double t_start;

    /*!
     * \brief Ping.
     *
     * Scheduled time to broadcast a ping.
     */
    double t_ping;

    /*!
     * \brief Self-destruction.
     *
     * Scheduled time of self-destruction.
     */
    double t_end;

    /*!
     * \brief Initial velocity.
     *
     * Velocity during launch.
     */
    double v;

    /*!
     * \brief Initial orientation.
     *
     * Orientation during launch.
     */
    double psi;
};

/*!
 * \brief Observation of a missile.
 *
 * Missiles are observed in space and time either by their scheduled ping or
 * when they detonate at the surface of a planet.
 */
struct GrvxMissileObservation {
    /*!
     * \brief Planet ID if a planet was hit.
     *
     * Planet ID if a planet was hit. Otherwise this is set to a value larger
     * than the number of planets.
     */
    uint32_t planet_id;

    /*
     * \brief Tick of observation.
     *
     * Point in time of scheduled ping or detonation at a planet surface.
     */
    double t;

    /*!
     * \brief Latitude.
     *
     * Latitudinal position.
     */
    double lat;

    /*!
     * \brief Longitude.
     *
     * Longitudinal position.
     */
    double lon;
};

/*!
 * \brief Initializes planets randomly.
 *
 * Planets are randomly distributed in their universe by sampling from a
 * distribution that is uniform on spherical surfaces. If the distance of two
 * planets is smaller than \p min_dist, new positions are drawn from the
 * distribution until all planets are (pair-wise) separated by at least the
 * requested distance.
 *
 * The number of random draws from the distribution is returned. Note that this
 * number equals at least the number of planets and is larger whenever
 * additional draws were needed.
 *
 * The seed is updated by each draw such that two consecutive calls to
 * grvx_rnd_init_planets() (w/o resetting \p seed) will return different
 * results.
 *
 * If \p min_dist is chosen too large, initializing can cause many iterations
 * and can even block indefinitely. As a rule of thumb,
 * \f$\cos d/2 \gg 1 - 2/n\f$ or \f$d \ll 4 / \sqrt{n}\f$ where \f$d\f$ is
 * \p min_dist in radians, \f$n\f$ is the number of planets, and
 * \f$2 \pi (1 - \cos r)\f$ is the area of a small circle with radius \f$r\f$ on
 * a unit sphere.
 *
 * @param planets The planets handle.
 * @param seed Seed value used for generating random values.
 * @param min_dist Minimum distance between two planets.
 * @return Number of random draws from the distribution.
 */
GRVX_EXPORT int32_t grvx_rnd_init_planets(GrvxPlanetsHandle planets,
                                          uint32_t *seed,
                                          double min_dist);

/*!
 * \brief Initializes a new game from a set of planets.
 *
 * A new game is created from a set of (already initialized) planets.
 *
 * The caller of this functions owns this new game instance and it is his/her
 * obligation to eventually destroy it via grvx_delete_game().
 *
 * @param planets Set of planets.
 * @return Game handle.
 */
GRVX_EXPORT GrvxGameHandle grvx_init_game(GrvxPlanetsHandle planets);

/*!
 * \brief Deletes a game instance.
 *
 * Deletes a game instance that was created with grvx_init_game(), frees
 * allocated memory, and invalidates any pointers to GrvxMissileObservation that
 * were returned by grvx_observe_or_tick().
 *
 * @param handle The game handle.
 */
GRVX_EXPORT void grvx_delete_game(GrvxGameHandle handle);

/*!
 * \brief Requests a missile launch.
 *
 * Requests the launch of a missile at some future time step (tick.) If the
 * request is accepted, a launch is scheduled at the given planet.
 *
 * The time step of the integrator, \f$h\f$, is set to the ratio of \p dt over
 * the product of the number of integration steps between trajectory points,
 * ``GRVX_INT_STEPS``, and the trajectory size ``GRVX_TRAJECTORY_SIZE``.
 * This makes \p dt and the simulated time interval independent of
 * ``GRVX_INT_STEPS`` and ``GRVX_TRAJECTORY_SIZE`.
 *
 * @param game The game handle.
 * @param planet_id ID of the planet.
 * @param missile Settings of the requested missile launch.
 * @param dt Step size of the simulation.
 * @return Zero on success.
 */
GRVX_EXPORT int32_t grvx_request_launch(GrvxGameHandle game,
                                        uint32_t planet_id,
                                        struct GrvxMissileLaunch *missile,
                                        double dt);

/*!
 * \brief Returns a new observation or advances time.
 *
 * Pings and planet destructions are observations and a list of observations is
 * associated with each tick. Upon calling one element of the list for the
 * current tick is returned and is removed from the list. The current tick is
 * written to \p t.
 * If the list is empty, time is advanced by increasing the tick count by one.
 * This updated tick is written to \p t and ``NULL`` is returned.
 *
 * The lifetime of the returned observation is managed by the game instance. In
 * particular, calling grvx_observe_or_tick() implicitly invalidates any
 * previous pointers that were returned by grvx_observe_or_tick(). Calling
 * grvx_delete_game() invalidates all pointers to GrvxMissileObservation that
 * were returned by grvx_observe_or_tick().
 *
 * @param game The game handle.
 * @param t Updated and now current tick (time step.)
 * @return Pointer to observation or ``NULL``.
 */
GRVX_EXPORT struct GrvxMissileObservation *
grvx_observe_or_tick(GrvxGameHandle game, uint32_t *t);

#ifdef __cplusplus
} // extern "C"
#endif

/*! @} */ // end of group API

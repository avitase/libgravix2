/*!
 * \file api.h
 * \brief API
 *
 * See module \ref API for a detailed description.
 */

/*!
 * \defgroup API API
 * \brief API for creating planets and launching missiles.
 *
 * @{
 *
 * The API of gravix2's physics engine should be used to create planets and
 * launch missiles. Upon calling propagate_missile(), missiles move on the
 * surface of a unit sphere and in a conservative gravitational force field
 * generated by static planets.
 *
 * In order to simulate the propagation of missiles, planets have to be defined.
 * Use new_planets() and set_planet() to create and initialize new planets and
 * pass the handle returned by the former when required by other functions. It
 * is the obligation of the caller to free allocated memory when necessary by
 * calling delete_planets() for planets and delete_missiles() for missiles.
 *
 * Missiles are created via new_missiles(). Note that new_planets() should be
 * used to craft separate universes, whereas multiple calls to new_missiles()
 * can be used in the same universe (referred to by the same planet handle.)
 * However, each call to new_missiles() will dynamically allocate memory and
 * thus comes with a computational overhead during runtime.
 *
 * Each missile is represented by a Trajectory parametrized by a sequence of
 * spatial and velocity data, represented in Cartesian coordinates. In order
 * to transform this parametrization back into a spherical parametrization using
 * latitude and longitude pairs, the helper functions lat(), lon(), vlat() and
 * vlon() can be used. **Note that spherical arguments have to be given in
 * units of degrees.**
 *
 * For universes with a single planet, missiles launched at this planet will
 * always come back. If launched with a sufficiently large momentum, missiles
 * propagate on a great circle of the unit sphere and eventually hit the planet
 * on the opposite side. We refer to this critical momentum as the escape
 * velocity, v_esc(), and to the integrated time between launching the missile
 * and its collision with the planet as the orbital periode, orb_period().
 */

#ifndef PHYSICS_API_H
#define PHYSICS_API_H

#include "config.h"

struct Planets;

/*!
 * \brief Handle to a set of planets forming a closed universe.
 *
 * Planets are kept intentionally opaque in the API and should only be referred
 * to by their respective PlanetsHandler. Internals are subjects to change.
 */
typedef struct Planets *PlanetsHandle;

/*!
 * \brief Sequence of spatial and velocity data of a missile.
 *
 * Note that the size of the trajectory is returned by propagate_missile().
 */
struct Trajectory {
    double x[TRAJECTORY_SIZE][3]; /*!< Cartesian position. */
    double v[TRAJECTORY_SIZE][4]; /*!< Normalized velocity and magnitude. */
};

/*!
 * \brief Creates a new planets handle.
 *
 * Allocates a new set of planets and returns a handle. It is the obligation of
 * the caller to initialize all planets before propagating missiles and to
 * prevent memory leakage by deleting all planets with delete_planets().
 *
 * @param n Number of planets.
 * @return Planets handle to yet uninitialized planets.
 */
PlanetsHandle new_planets(unsigned n);

/*!
 * \brief Deletes all planets of the associated handle and frees the allocated
 * memory.
 *
 * @param handle The planets handle.
 */
void delete_planets(PlanetsHandle handle);

/*!
 * \brief Initializes or overwrites the spatial position of a planet.
 *
 * Planets are referred to by unique IDs. For \f$n\f$ planets (cf.
 * new_planets()) the IDs are the integer values \f$[0, 1, 2, \ldots, n-1]\f$.
 * Note that all planets of a universe have to be initialized before missiles
 * are propagated.
 *
 * @param handle The planets handle.
 * @param i ID of the planet to be initialized.
 * @param lat Latitude in degrees.
 * @param lon Longitude in degrees.
 * @return Zero on success.
 */
int set_planet(PlanetsHandle handle, unsigned i, double lat, double lon);

/*!
 * \brief Removes the last planet from the universe.
 *
 * The last planet in the universe is the one with the largest ID. If called
 * \f$k\f$ times for a universe with \f$n\f$ planets, \f$n-k\f$ planets remain.
 * If \f$k > n\f$ the behavior is undefined.
 *
 * @param handle The planets handle.
 * @return Number of remaining planets in the universe.
 */
unsigned pop_planet(PlanetsHandle handle);

/*!
 * \brief Handle to a batch of trajectories.
 *
 * For the sake of minimizing the number of memory allocation, trajectories can
 * be allocated in batches. A batch is referred to as a handle to trajectories.
 */
typedef struct Trajectory *TrajectoryBatch;

/*!
 * \brief Creates a new batch of uninitialized missiles.
 *
 * Use get_trajectory() and either init_missile() or launch_missile() to
 * initialize missiles. Propagating uninitialized missiles is undefined.
 *
 * @param n Number of missiles to be bundled into a batch.
 * @return The handle to the batch.
 */
TrajectoryBatch new_missiles(unsigned n);

/*!
 * \brief Deletes a batch of missiles and frees the allocated memory.
 *
 * @param batch The handle to the batch.
 */
void delete_missiles(TrajectoryBatch batch);

/*!
 * \brief Extracts a missile by ID from the given missile batch.
 *
 * Missiles are referred to by unique and static IDs within a batch. For a batch
 * of size \f$n\f$ (cf. new_missiles()) the IDs are the integer values \f$[0, 1,
 * 2, \ldots, n-1]\f$.
 *
 * @param batch The handle to the missile batch.
 * @param i The missile ID. The behavior of requesting invalid IDs is undefined.
 * @return The trajectory handle of the requested missile.
 */
struct Trajectory *get_trajectory(TrajectoryBatch batch, unsigned i);

/*!
 * \brief Initializes the position and velocity of a missile on the sphere.
 *
 * The missile is aligned on the surface of the sphere s.t. it is pointing
 * towards \f$\Delta \phi\f$ and \f$(\Delta \lambda \, \cos \phi)\f$ where the
 * former (latter) is the difference in latitude (longitude). The magnitude of
 * the vector is given separately by \p v.
 *
 * For convenience, this function will also write the initial values into the
 * first slot of the trajectory (index ``0``) where they can be read out safely.
 * Note that this value is overwritten by calling propagate_missile().
 *
 * @param trj The trajectory handle as obtained from get_trajectory().
 * @param lat The initial latitude, \f$\phi\f$, in units of degrees.
 * @param lon The initial longitude, \f$\lambda\f$, in units of degrees.
 * @param v The intitial velocity, \f$\sqrt{\dot\phi^2 + (\dot\lambda \,
 * \cos\phi)^2}\f$.
 * @param dlat The intitial latitudinal orientation, \f$\Delta \phi\f$.
 * @param dlon The intitial longitudinal orientation, \f$\Delta \lambda \,
 * \cos\phi\f$.
 * @return Zero on success.
 */
int init_missile(struct Trajectory *trj,
                 double lat,
                 double lon,
                 double v,
                 double dlat,
                 double dlon);

/*!
 * \brief Initializes a missile on the rim of a given planet.
 *
 * The missile is placed on the rim of the planet with a radial bearing pointing
 * away from the planet center if the initial velocity is positive. The rim is
 * defined as a circle centered at the planet. Propagation of missiles that fall
 * inside this circle is stopped, cf. propagate_missile().
 *
 * For convenience, this function will also write the initial values into the
 * first slot of the trajectory (index ``0``) where they can be read out safely.
 * Note that this value is overwritten by calling propagate_missile().
 *
 * @param trj The trajectory handle as obtained from get_trajectory().
 * @param planets_handle The planets handle.
 * @param planet_id The planet ID.
 * @param v Magnitude of the initial velocity, \f$\sqrt{\dot\phi^2 +
 * (\dot\lambda \cos\phi)^2}\f$, where \f$\phi\f$ and \f$\lambda\f$ are the
 * latitudinal and longitudinal position on the rim in degrees, respectively,
 * and the dot indicates temporal derivatives.
 * @param psi The azimuthal position on the rim in units of degrees.
 * @return Zero on success.
 */
int launch_missile(struct Trajectory *trj,
                   const PlanetsHandle planets_handle,
                   unsigned planet_id,
                   double v,
                   double psi);

/*!
 * \brief Propagates a missile in the gravitational force field of planets.
 *
 * Before calling this method for the first time each missile has to be
 * initialized by either using init_missile() or launch_missile(). Afterwards,
 * multiple calls to this method can be chained until the \p premature flag is
 * set. The behavior of any subsequent calls is undefined until a missile is
 * reinitialized using either init_missile() or launch_missile().
 *
 * Consider a trajectory sequence ``[x, x+1, x+2, ..., x+N]`` where ``x`` refers
 * to some point in phase space and ``x+1`` to its immediate successor.
 * Let ``2`` be the return value of this function, then the updated sequence
 * corresponds to ``[x+N+1, x+N+2, x+2, ..., x+N]``, i.e., the first two values
 * were updated by the integrator. In particular, initial values at the first
 * position are overwritten upon calling.
 *
 * If a missile propagates inside the rim of a planet, cf. launch_missile(), the
 * \p premature flag is set to a non-zero value and propagation is stopped.
 *
 * @param trj The trajectory handle as obtained from get_trajectory().
 * @param planets The planets handle.
 * @param h The step size of the integrator. Typically, this value should be
 * \f$\ll 1\f$.
 * @param premature Set to non-zero values if propagation was stopped
 * prematurely. If set this also indicates that the last time interval between
 * consecutive steps of the trajectory is shorter than those of the other
 * consecutive steps.
 * @return Number of simulated steps stored into the trajectory sequence. This
 * value should be used to safely readout the data from Trajectory, i.e., let
 * \f$n\f$ be the returned value, then the first \f$n\f$ points of \p trj were
 * updated by the integrator. Note that this value does not change if the
 * missile passes a planet rim within the last integration step! (The
 * \p premature flag is still set in this case.)
 */
unsigned propagate_missile(struct Trajectory *trj,
                           const PlanetsHandle planets,
                           double h,
                           int *premature);

/*!
 * \brief Computes the latitudinal position, \f$\phi\f$, from Cartesian
 * coordinates.
 *
 * @param z (Third) \f$z\f$-coordinate of Cartesian representation.
 * @return Latitude, \f$\phi\f$, in units of degrees.
 */
double lat(double z);

/*!
 * \brief Computes the longitudinal position, \f$\lambda\f$, from Cartesian
 * coordinates.
 *
 * @param x (First) \f$x\f$-coordinate of Cartesian representation.
 * @param y (Second) \f$y\f$-coordinate of Cartesian representation.
 * @return Longitude, \f$\lambda\f$, in units of degrees.
 */
double lon(double x, double y);

/*!
 * \brief Computes the latitudinal speed, \f$\dot\phi\f$, from Cartesian
 * coordinates.
 *
 * @param vx (First) \f$x\f$-component of velocity in Cartesian representation.
 * @param vy (Second) \f$y\f$-component of velocity in Cartesian representation.
 * @param vz (Third) \f$z\f$-component of velocity in Cartesian representation.
 * @param lat Latitude, \f$\phi\f$, in degrees as obtained from lat().
 * @param lon Longitude, \f$\lambda\f$, in degrees as obtained from lon().
 * @return Latitudinal speed, \f$\dot\phi\f$.
 */
double vlat(double vx, double vy, double vz, double lat, double lon);

/*!
 * \brief Computes the (scaled) longitudinal speed, \f$\dot\lambda \cos\phi\f$,
 * from Cartesian coordinates.
 *
 * @param vx (First) \f$x\f$-component of velocity in Cartesian representation.
 * @param vy (Second) \f$y\f$-component of velocity in Cartesian representation.
 * @param vz (Third) \f$z\f$-component of velocity in Cartesian representation.
 * @param lon Longitude, \f$\lambda\f$, in degrees as obtained from lon().
 * @return (Scaled) longitudinal speed, \f$\dot\lambda \cos\phi\f$.
 */
double vlon(double vx, double vy, double vz, double lon);

/*!
 * \brief Escape velocity \f$p_\pi\f$ for isolated planets.
 *
 * For isolated planets: Minimal initial momentum necessary to asymptotically
 * reach a distance of \f$\pi\f$ to the center of the planet if launched on its
 * rim.
 *
 * @return Escape velocity, \f$p_\pi\f$.
 */
double v_esc(void);

/*!
 * \brief Orbital period for isolated planets.
 *
 * For isolated planets: Integrated time between launching and touching the rim
 * of the planet for the first time. If \p v0 > v_esc() the missile propagates
 * on a great circle of the unit sphere and collides with the planet after an
 * integrated path length of \f$2(\pi - \delta)\f$ where \f$\delta\f$ is the
 * radius of the planet's rim.
 *
 * This method returns a more precise value than one would get by calling
 * propagate_missile() until the \p premature flag is set.
 *
 * @param v0 Initial speed.
 * @param h The step size of the integrator. Typically, this value should be
 * \f$\ll 1\f$.
 * @return Orbital period.
 */
double orb_period(double v0, double h);

/*!
 * \brief Static configurations.
 *
 * There are several static configurations that can be set at compile-time.
 * Use get_config() to get the configuration that was used during compilation.
 * The corresponding compile-time constants that can be passed to <a
 * href="https://cmake.org">CMake</a> are:
 *
 *  - ``POT_TYPE``: same as Config::pot_type
 *  - ``N_POT``: same as Config::n_pot
 *  - ``TRAJECTORY_SIZE``: same as Config::trajectory_size
 *  - ``INT_STEPS``: same as Config::int_steps
 *  - ``MIN_DIST``: same as Config::min_dist
 *  - ``P_MIN``: same as Config::p_min
 *  - ``COMPOSITION_SCHEME``: same as Config::composition_scheme
 *
 * Note that none of these settings is necessarily needed to interact with the
 * API, for example, propagate_missile() returns the number of simulated steps
 * stored in the trajectory sequence upon calling. In fact, it is safer to
 * prefer this value over Config::trajectory_size because the former also
 * reflects cases where less values were stored in case of a collision with a
 * planet.
 */
struct Config {
    /*!
     * \brief Type of potential.
     *
     * Currently, we support two types of potentials, \f$V_{2\mathrm{D}}\f$ and
     * \f$V_{3\mathrm{D}}\f$ and identify them by the strings ``"2D"`` and
     * ``"3D"``, respectively.
     *
     * Possible values: ``"2D"`` and ``"3D"``.
     */
    const char *pot_type;

    /*!
     * \brief Approximation order of potential.
     *
     * There is no closed-form for \f$V_{3\mathrm{D}} = \frac{1}{4\pi}
     * \sum_{j=0}^\infty V^{(j)}_{3\mathrm{D}}\f$, but the series can be treated
     * as a perturbation expansion with \f$V^{(j)}_{3\mathrm{D}} >
     * V^{(j')}_{3\mathrm{D}}\f$ if \f$j < j'\f$. Empirically, we find that
     * \f[
     *  V_{3\mathrm{D}}
     *  \approx \frac{1}{4\pi} \sum_{j=0}^{\texttt{n_pot}-1}
     *  V^{(j)}_{3\mathrm{D}}
     * \f]
     * converges fast and yields decent approximations.
     *
     * For \f$V_{2\mathrm{D}}\f$ a closed-form expression does exist and no
     * approximation is needed.
     *
     * Possible values: Positive integer values or undefined if Config::pot_type
     * is ``"2D"``.
     */
    int n_pot;

    /*!
     * \brief Size of trajectory.
     *
     * Missiles are propagated for multiple time steps. The result is stored in
     * arrays with \p trajectory_size elements. See Trajectory for more details.
     *
     * Possible values: Positive, non-zero integer values.
     */
    int trajectory_size;

    /*!
     * \brief Number of integration steps between trajectory points.
     *
     * Number of integration steps between consecutive points of a trajectory.
     * Note that changing this number effectively speeds-up or slows-down the
     * simulated and simulation time.
     *
     * Possible values: Positive, non-zero integer values.
     */
    int int_steps;

    /*!
     * \brief Min. allowed distance between missiles and planets.
     *
     * If missiles approach planets closer than this minimal distance (given in
     * degrees), propagation is stopped.
     *
     * Possible values: Positive floating point values.
     */
    double min_dist;

    /*!
     * \brief Momentum threshold.
     *
     * If the momentum of a missile is lower than this threshold for two
     * consecutive integration steps, the momentum is set to zero to ensure
     * numeric stability.
     *
     * Possible values: Positive floating point values.
     */
    double p_min;

    /*!
     * \brief Composition method of integrator
     *
     * The composition method of the integrator encoded as ``"pXsY"`` where
     * ``X`` is the integration order and ``Y`` is the number of stages.
     *
     * Possible values: \link P2GAMMA_p2s1 ``"p2s1"`` \endlink, \link
     * P2GAMMA_p4s3 ``"p4s3"`` \endlink, \link P2GAMMA_p4s5 ``"p4s5"`` \endlink,
     * \link P2GAMMA_p6s9 ``"p6s9"`` \endlink and \link P2GAMMA_p8s15
     * ``"p8s15"`` \endlink.
     */
    const char *composition_scheme;
};

/*!
 * \brief Writes static configuration.
 *
 * **NOT YET IMPLEMENTED**
 */
void get_config(struct Config *cfg);

#endif // PHYSICS_API_H

/*! @} */ // end of group API

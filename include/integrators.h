/*!
 * \file integrators.h
 * \brief Symmetric and symplectic integrator for ODEs
 */

#ifndef GRVX_PHYSICS_INTEGRATORS_H
#define GRVX_PHYSICS_INTEGRATORS_H

#include "linalg.h"

struct GrvxPlanets;

/*!
 * \brief Phase space representation of state.
 */
struct GrvxQP {
    struct GrvxVec3D q; /*!< Vector of canonical coordinates. */
    struct GrvxVec3D p; /*!< Vector of conjugate momenta. */
};

/*!
 * \brief Single integration step.
 *
 * \p qp is advanced by the integrator for a single step. Accumulation errors
 * are compensated by tracking them in \p e. Initialize it with zeros for the
 * first iteration.
 *
 * @param qp Phase space.
 * @param e Accumulation error.
 * @param h Step size.
 * @param planets Planets handle.
 */
void grvx_integration_step(struct GrvxQP *qp,
                           struct GrvxQP *eq,
                           double h,
                           const struct GrvxPlanets *planets);

/*!
 * \brief Multiple integration steps.
 *
 * \p qp is advanced by the integrator for a maximum of \p n steps. The
 * propagation is stopped if the minimum distance to any planet is reached.
 *
 * @param qp Phase space.
 * @param h Step size passed to grvx_integration_step().
 * @param n Maximum number of integration steps.
 * @param planets Planets handle.
 * @return Number of processed integration steps.
 */
unsigned grvx_integration_loop(struct GrvxQP *qp,
                               double h,
                               unsigned n,
                               const struct GrvxPlanets *planets);

#endif // GRVX_PHYSICS_INTEGRATORS_H

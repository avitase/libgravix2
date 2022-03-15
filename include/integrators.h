/*!
 * \file integrators.h
 * \brief Symmetric and symplectic integrator for ODEs
 */

#ifndef PHYSICS_INTEGRATORS_H
#define PHYSICS_INTEGRATORS_H

#include "linalg.h"

struct Planets;

/*!
 * \brief Phase space representation of state.
 */
struct QP {
    struct Vec3D q; /*!< Vector of canonical coordinates. */
    struct Vec3D p; /*!< Vector of conjugate momenta. */
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
void integration_step(struct QP *qp,
                      struct QP *eq,
                      double h,
                      const struct Planets *planets);

/*!
 * \brief Multiple integration steps.
 *
 * \p qp is advanced by the integrator for a maximum of \p n steps. The
 * propagation is stopped if the minimum distance to any planet is reached.
 *
 * @param qp Phase space.
 * @param h Step size passed to integration_step().
 * @param n Maximum number of integration steps.
 * @param planets Planets handle.
 * @return Number of processed integration steps.
 */
unsigned integration_loop(struct QP *qp,
                          double h,
                          unsigned n,
                          const struct Planets *planets);

#endif // PHYSICS_INTEGRATORS_H

/*!
 * \file pot.h
 * \brief Gradient estimations of the potential.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct GrvxVec3D;
struct GrvxPlanets;

/*!
 * \brief Gradient of the potential at position \p q.
 *
 * The gradient is evaluated at position \p q and the resulting vectors is
 * stored in \p q.
 *
 * @param q The position where the gradient is evaluated. The result overwrites
 * this variable.
 * @param planets Planets that generate the force field.
 */
void grvx_gradV(struct GrvxVec3D *q, const struct GrvxPlanets *planets);

/*!
 * \brief Minimal distance to any planet.
 *
 * @param q The position of the missile.
 * @param planets Planets that generate the force field.
 * @return Cosine of smallest angle between \p q and any planet.
 */
double grvx_min_dist(const struct GrvxVec3D *q,
                     const struct GrvxPlanets *planets);

#ifdef __cplusplus
} // extern "C"
#endif

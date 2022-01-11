/*!
 * \file pot.h
 * \brief Gradient estimations of the potential.
 */

#ifndef PHYSICS_POT_H
#define PHYSICS_POT_H

struct Vec3D;
struct Planets;

/*!
 * \brief Gradient of the potential at position \p q.
 *
 * The gradient is evaluated at position \p q and the resulting vectors is
 * stored in \p q.
 *
 * @param q The position where the gradient is evaluated. The result overwrites
 * this variable.
 * @param planets Planets handle which generates the force field.
 * @return Cosine of smallest angle between \p q and any planet.
 */
double gradV(struct Vec3D *q, const struct Planets *planets);

#endif // PHYSICS_POT_H

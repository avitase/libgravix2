/*!
 * \file linalg.h
 * \brief Helper functions for linear algebra calculations.
 */

#ifndef PHYSICS_LINALG_H
#define PHYSICS_LINALG_H

/*!
 * \brief 3D vector in cartesian representation.
 */
struct Vec3D {
    double x, y, z;
};

/*!
 * \brief Dot product of \p a and \p b.
 *
 * @param a First vector.
 * @param b Second vector.
 * @return Dot product of first and second vector.
 */
double dot(struct Vec3D a, struct Vec3D b);

#endif // PHYSICS_LINALG_H

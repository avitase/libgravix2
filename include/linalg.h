/*!
 * \file linalg.h
 * \brief Helper functions for linear algebra calculations.
 */

#ifndef GRVX_PHYSICS_LINALG_H
#define GRVX_PHYSICS_LINALG_H

/*!
 * \brief 3D vector in cartesian representation.
 */
struct GrvxVec3D {
    double x, y, z;
};

/*!
 * \brief Dot product of \p a and \p b.
 *
 * @param a First vector.
 * @param b Second vector.
 * @return Dot product of first and second vector.
 */
double grvx_dot(struct GrvxVec3D a, struct GrvxVec3D b);

/*!
 * \brief Magnitude of vector.
 *
 * @param v Vector.
 * @return Magnitude of vector.
 */
double grvx_mag(struct GrvxVec3D v);

#endif // GRVX_PHYSICS_LINALG_H

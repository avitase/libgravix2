/*!
 * \file planet.h
 * \brief Definition of the set of planets.
 */

#ifndef PHYSICS_PLANET_H
#define PHYSICS_PLANET_H

/*!
 * \brief Set of planets.
 *
 * The spatial position of each planet is stored as a tuple of the cartesian
 * coordinates \f$(x_1, y_1, z_1, x_2, y_2, z_2, \ldots x_n, y_n, z_n)\f$ in
 * \ref Planets.data, where \f$(x_i, y_i, z_i)\f$ refers to the \f$i\f$-th
 * planet.
 */
struct Planets {
    unsigned n;   /*!< Number of planets, \f$n\f$. */
    double *data; /*!< Data array. */
};

#endif // PHYSICS_PLANET_H

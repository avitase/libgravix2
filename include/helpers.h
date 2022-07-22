/*!
 * \file helpers.h
 * \brief Compilation of helper functions.
 */

#ifndef GRVX_HELPERS_H
#define GRVX_HELPERS_H

/*!
 * \brief Computes \f$\sin(x) / x\f$.
 *
 * Save calculation of \f$\sin(x) / x\f$. In particular, this function returns
 * one for \f$x=0\f$.
 *
 * @param x \f$x\f$.
 * @return \f$\sin(x) / x\f$.
 */
double grvx_sinc(double);

#endif // GRVX_HELPERS_H

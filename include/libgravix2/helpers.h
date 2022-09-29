/*!
 * \file helpers.h
 * \brief Compilation of helper functions.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Computes \f$\sin(x) / x\f$.
 *
 * Save calculation of \f$\sin(x) / x\f$. In particular, this function returns
 * 1.0 for \f$x=0\f$.
 *
 * @param x \f$x\f$.
 * @return \f$\sin(x) / x\f$.
 */
double grvx_sinc(double);

#ifdef __cplusplus
} // extern "C"
#endif

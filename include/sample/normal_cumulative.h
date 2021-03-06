/* *
 * Copyright (C) 2018 XLAB d.o.o.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of either:
 *
 *     * the GNU Lesser General Public License as published by the Free
 *     Software Foundation; either version 3 of the License, or (at your
 *     option) any later version.
 *
 * or
 *
 *     * the GNU General Public License as published by the Free Software
 *     Foundation; either version 2 of the License, or (at your option) any
 *     later version.
 *
 * or both in parallel, as here.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CIFER_NORMAL_CUMULATIVE_H
#define CIFER_NORMAL_CUMULATIVE_H

#include "sample/normal.h"

/**
 * \file
 * \ingroup sample
 * \brief Normal cumulative sampler
 */

/**
 * cfe_normal_cumulative samples random values from the cumulative normal (Gaussian)
 * probability distribution, centered on 0.
 * This sampler is the fastest, but is limited only to cases when sigma is not
 * too big, due to the sizes of the precomputed tables.
 */
typedef struct cfe_normal_cumulative {
    cfe_normal nor;
    cfe_vec precomputed;    // table of precomputed values relative to the cumulative distribution
    bool two_sided;         // twoSided defines if we limit sampling only to non-negative integers or to all
    mpz_t sample_size;      // integer defining from how big of an interval do we need to sample uniformly to sample according to discrete Gauss
} cfe_normal_cumulative;

/**
 * Initializes an instance of cfe_normal_cumulative sampler. It assumes mean = 0.
 * Values are precomputed when this function is called, so that
 * normal_cumulative_sample merely returns a precomputed value.
 *
 * @param s A pointer to an uninitialized struct representing the sampler
 * @param sigma Standard deviation
 * @param n Precision parameter
 * @param two_sided Boolean defining if we limit only to non-negative integers
 */
void cfe_normal_cumulative_init(cfe_normal_cumulative *s, mpf_t sigma, size_t n, bool two_sided);

/**
 * Frees the memory occupied by the struct members. It does not free
 * memory occupied by the struct itself.
 *
 * @param s A pointer to an instance of the sampler (*initialized*
 * cfe_normal_cumulative struct)
 */
void cfe_normal_cumulative_free(cfe_normal_cumulative *s);

/**
 * Samples discrete cumulative distribution with precomputed values.
 *
 * @param res The random number (result value will be stored here)
 * @param s A pointer to an instance of the sampler (*initialized*
 * cfe_normal_cumulative struct)
 */
void cfe_normal_cumulative_sample(mpz_t res, cfe_normal_cumulative *s);

/**
 * Precomputes the values for sampling. This can be used only if sigma is not
 * too big.
 *
 * @param s A pointer to an instance of the sampler (*initialized*
 * cfe_normal_cumulative struct)
 */
void cfe_normal_cumulative_precompute(cfe_normal_cumulative *s);

/**
 * Given a random value from an interval and a cumulative distribution of mpz_t
 * values, it returns a sample distributed accordingly using binary search
 *
 * @param sample Target integer
 * @param start Start index
 * @param end End index
 * @param v Vector of values
 * @param middle_value Temporary placeholder for value comparison (to avoid
 * unnecessary memory allocations)
 * @return index of target integer
 */
size_t cfe_locate_int(mpz_t sample, size_t start, size_t end, cfe_vec *v, mpz_t middle_value);

/**
 * Sets the elements of a vector to random numbers with the normal_cumulative sampler.
 */
void cfe_normal_cumulative_sample_vec(cfe_vec *res, cfe_normal_cumulative *s);

/**
 * Sets the elements of a matrix to random numbers with the normal_cumulative sampler.
 */
void cfe_normal_cumulative_sample_mat(cfe_mat *res, cfe_normal_cumulative *s);

#endif

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

#include <gmp.h>
#include "test.h"
#include "data/vec.h"
#include "innerprod/fullysec/paillier.h"
#include "sample/uniform.h"

MunitResult test_paillier_end_to_end(const MunitParameter *params, void *data) {
    size_t l = 50;
    size_t lambda = 128;
    size_t bit_len = 512;
    mpz_t bound_x, bound_y, derived_key, xy_check, xy, bound_x_neg, bound_y_neg;
    mpz_inits(bound_x, bound_y, xy_check, bound_x_neg, bound_y_neg, NULL);
    mpz_set_ui(bound_x, 2);
    mpz_pow_ui(bound_x, bound_x, 10);
    mpz_set(bound_y, bound_x);
    mpz_neg(bound_x_neg, bound_x);
    mpz_add_ui(bound_x_neg, bound_x_neg, 1);
    mpz_neg(bound_y_neg, bound_y);
    mpz_add_ui(bound_y_neg, bound_y_neg, 1);

    cfe_paillier s, encryptor;
    cfe_error err = cfe_paillier_init(&s, l, lambda, bit_len, bound_x, bound_y);
    munit_assert(err == 0);

    cfe_vec msk, mpk, ciphertext, x, y;
    cfe_vec_inits(l, &x, &y, NULL);
    cfe_uniform_sample_range_vec(&x, bound_x_neg, bound_x);
    cfe_uniform_sample_range_vec(&y, bound_y_neg, bound_y);
    cfe_vec_dot(xy_check, &x, &y);

    cfe_paillier_generate_master_keys(&msk, &mpk, &s);

    err = cfe_paillier_derive_key(derived_key, &s, &msk, &y);
    munit_assert(err == 0);

    cfe_paillier_copy(&encryptor, &s);
    err = cfe_paillier_encrypt(&ciphertext, &encryptor, &x, &mpk);
    munit_assert(err == 0);

    err = cfe_paillier_decrypt(xy, &s, &ciphertext, derived_key, &y);
    munit_assert(err == 0);

    munit_assert(mpz_cmp(xy, xy_check) == 0);

    mpz_clears(bound_x, bound_y, derived_key, xy_check, xy, bound_x_neg, bound_y_neg, NULL);
    cfe_vec_frees(&x, &y, &msk, &mpk, &ciphertext, NULL);

    cfe_paillier_free(&s);
    cfe_paillier_free(&encryptor);

    return MUNIT_OK;
}

MunitTest simple_ip_paillier_tests[] = {
        {(char *) "/end-to-end", test_paillier_end_to_end, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {NULL, NULL,                                       NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

MunitSuite paillier_suite = {
        (char *) "/innerprod/fullysec/paillier", simple_ip_paillier_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};


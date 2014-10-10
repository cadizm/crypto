
#include <stdint.h>
#include <math.h>
#include <arpa/inet.h>
#include <openssl/bn.h>
#include <openssl/err.h>

#include "maurer.h"
#include "trialdiv.h"
#include "util.h"


/*
 * Internal recursive maurer
 */
static void maurer_(int level, int k, FILE* fpPrimes, FILE* fpRnd, BIGNUM* bn_res);


/*
 * Public maurer function
 */
void maurer(int level, int k, FILE* fpPrimes, FILE* fpRnd)
{
    uint32_t error = 0;
    BIGNUM* bn_n = NULL;

    if (!(bn_n = BN_new())) goto end;

    maurer_(level, k, fpPrimes, fpRnd, bn_n);

    printf("\nMaurer's Algorithm found an %d-bit prime:\n", k);
    printf("  n = %s\n", BN_bn2dec(bn_n));

end:

    error = ERR_get_error();

    if (error) {
        const char* strError = ERR_error_string(error, NULL);
        fprintf(stderr, "%s\n", strError);
        exit(-1);
    }
}

void maurer_(int level, int k, FILE* fpPrimes, FILE* fpRnd, BIGNUM* bn_res)
{
    uint32_t error = 0;

    int m = 0;
    double r = 0.0;

    double B = 0.0;
    double c = 0.0;

    BIGNUM* bn_n = NULL;
    BIGNUM* bn_n_minus_one = NULL;
    BIGNUM* bn_I = NULL;
    BIGNUM* bn_R = NULL;
    BIGNUM* bn_R_mod_I = NULL;
    BIGNUM* bn_a = NULL;
    BIGNUM* bn_two_mul_R = NULL;
    BIGNUM* bn_b = NULL;
    BIGNUM* bn_b_minus_one = NULL;
    BIGNUM* bn_d = NULL;
    BIGNUM* bn_one = NULL;
    BIGNUM* bn_two = NULL;
    BIGNUM* bn_k_minus_two = NULL;
    BIGNUM* bn_two_pow_k_minus_two = NULL;
    BIGNUM* bn_two_R_mul_q = NULL;
    BN_CTX* bn_ctx = NULL;

    if (!(bn_n = BN_new())) goto end;
    if (!(bn_n_minus_one = BN_new())) goto end;
    if (!(bn_I = BN_new())) goto end;
    if (!(bn_R = BN_new())) goto end;
    if (!(bn_R_mod_I = BN_new())) goto end;
    if (!(bn_a = BN_new())) goto end;
    if (!(bn_two_mul_R = BN_new())) goto end;
    if (!(bn_b = BN_new())) goto end;
    if (!(bn_b_minus_one = BN_new())) goto end;
    if (!(bn_d = BN_new())) goto end;
    if (!(bn_one = BN_new())) goto end;
    if (!(bn_two = BN_new())) goto end;
    if (!(bn_k_minus_two = BN_new())) goto end;
    if (!(bn_two_pow_k_minus_two = BN_new())) goto end;
    if (!(bn_two_R_mul_q = BN_new())) goto end;
    if (!(bn_ctx = BN_CTX_new())) goto end;

    printf("Maurer: level %d, k=%d\n", level, k);

    if (k <= 20) {
        while (1) {
            rndOddNum(k, fpRnd, bn_n);

            printf("  step 1.1, n = %s\n", BN_bn2dec(bn_n));

            if ((m = trialdiv(bn_n, fpPrimes, 0)) == 0) {
                printf("    n passes trial division test\n");
                BN_copy(bn_res, bn_n);
                return;
            }
            else if (m > 0) {
                printf("    n is composite by trial division (mod %d = 0)\n", m);
            }
        }
    }

    c = 0.1;
    m = 20;
    B = c * pow((double)k, 2.0);

    if (k <= 2 * m) {
        r = 0.5;
        printf("  step 4, r = %d%%\n", round(r*100.0));
    }
    else {
        while (1) {
            uint8_t byte = rndByte(fpRnd);
            r =  byte / 255.0;
            r = 0.5 + r / 2.0;
            if (k * (1-r) > m) {
                printf("  step 4: random byte = %d, r = %d%%\n", (int)byte, round(r*100.0));
                break;
            }
        }
    }

    /* initialize constants */
    BN_one(bn_one);
    uint32_t word = 2;
    word = htonl(word);
    BN_bin2bn((uint8_t*)(&word), sizeof(uint32_t), bn_two);
    word = k - 2;
    word = htonl(word);
    BN_bin2bn((uint8_t*)(&word), sizeof(uint32_t), bn_k_minus_two);

    /* recursion */
    maurer_(level+1, ((int)floor(r*k))+1, fpPrimes, fpRnd, bn_res);
    int num_bits_in_q = BN_num_bits(bn_res);

    printf("Maurer: back to level %d, k=%d, q=%s\n", level, k, BN_bn2dec(bn_res));

    /* I = floor(2^(k-2) / q) */
    BN_exp(bn_two_pow_k_minus_two, bn_two, bn_k_minus_two, bn_ctx);
    BN_div(bn_I, NULL, bn_two_pow_k_minus_two, bn_res, bn_ctx);

    int iter = 0;

    while (1) {
        iter += 1;
        rndOddNum(k+1-num_bits_in_q, fpRnd, bn_R);

        /* R = (R mod I) + I + 1 */
        BN_mod(bn_R_mod_I, bn_R, bn_I, bn_ctx);
        BN_add(bn_R, bn_R_mod_I, bn_I);
        BN_add(bn_R, bn_R, bn_one);

        /* n = 2Rq+1 */
        BN_mul(bn_two_R_mul_q, bn_two, bn_R, bn_ctx);
        BN_mul(bn_two_R_mul_q, bn_two_R_mul_q, bn_res, bn_ctx);
        BN_add(bn_n, bn_two_R_mul_q, bn_one);

        printf("  step 7, itr %d: R = %s, n = %s\n", iter, BN_bn2dec(bn_R), BN_bn2dec(bn_n));

        m = trialdiv(bn_n, fpPrimes, 0);

        if (m > 0) {
            printf("    n is composite by trial division (mod %d = 0)\n", m);
        }

        else {
            printf("    n passes trial division test\n");
            int num_bits_in_n = BN_num_bits(bn_n);

            do {
                rndOddNum(num_bits_in_n, fpRnd, bn_a);
                BN_sub(bn_n_minus_one, bn_n, bn_one);
            } while ((BN_cmp(bn_a, bn_one) <= 0) ||
                     (BN_cmp(bn_a, bn_n_minus_one) >= 0));

            printf("  step 7.2.1, itr %d: a = %s\n", iter, BN_bn2dec(bn_a));

            /* b = a^{n-1} mod n */
            BN_mod_exp(bn_b, bn_a, bn_n_minus_one, bn_n, bn_ctx);

            if (BN_is_one(bn_b)) {
                /* b = a^{2R} mod n */
                BN_mul(bn_two_mul_R, bn_two, bn_R, bn_ctx);
                BN_mod_exp(bn_b, bn_a, bn_two_mul_R, bn_n, bn_ctx);

                /* d = gcd(b-1,n) */
                BN_sub(bn_b_minus_one, bn_b, bn_one);
                BN_gcd(bn_d, bn_b_minus_one, bn_n, bn_ctx);

                if (BN_is_one(bn_d)) {
                    BN_copy(bn_res, bn_n);
                    return;
                }
            }
        }
    }

end:

    error = ERR_get_error();

    if (error) {
        const char* strError = ERR_error_string(error, NULL);
        fprintf(stderr, "%s\n", strError);
        exit(-1);
    }
}

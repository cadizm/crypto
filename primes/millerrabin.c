
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <openssl/err.h>

#include "millerrabin.h"


int millerrabin(BIGNUM* bn_num, uint64_t maxitr, FILE* fp, int nIndent)
{
    rewind(fp);

    int flag = 0;
    uint32_t buf = 0;
    uint32_t error = 0;
    char* strIndent = NULL;

    strIndent = (char*)malloc((nIndent+1) * sizeof(char));
    memset(strIndent, '\0', nIndent+1);
    for (int i = 0; i < nIndent; ++i) {
        strcat(strIndent, " ");
    }


    BIGNUM* bn_one = NULL;
    BIGNUM* bn_two = NULL;
    BIGNUM* bn_a = NULL;
    BIGNUM* bn_y = NULL;
    BIGNUM* bn_y_sqr = NULL;
    BIGNUM* bn_j = NULL;
    BIGNUM* bn_s = NULL;
    BIGNUM* bn_s_minus_one = NULL;
    BIGNUM* bn_r = NULL;
    BIGNUM* bn_n_minus_one = NULL;
    BIGNUM* bn_two_pow_s = NULL;
    BIGNUM* bn_two_pow_s_mul_r = NULL;
    BN_CTX* bn_ctx = NULL;

    if (!(bn_one = BN_new())) goto end;
    if (!(bn_two = BN_new())) goto end;
    if (!(bn_a = BN_new())) goto end;
    if (!(bn_y = BN_new())) goto end;
    if (!(bn_y_sqr = BN_new())) goto end;
    if (!(bn_j = BN_new())) goto end;
    if (!(bn_s = BN_new())) goto end;
    if (!(bn_s_minus_one = BN_new())) goto end;
    if (!(bn_r = BN_new())) goto end;
    if (!(bn_n_minus_one = BN_new())) goto end;
    if (!(bn_two_pow_s = BN_new())) goto end;
    if (!(bn_two_pow_s_mul_r = BN_new())) goto end;
    if (!(bn_ctx = BN_CTX_new())) goto end;

    BN_one(bn_one);  /* set constant 1 */
    BN_one(bn_two);
    BN_add(bn_two, bn_one, bn_two);  /* set constant 2 */

    BN_sub(bn_n_minus_one, bn_num, bn_one);  /* set n-1 */
    BN_zero(bn_r);  /* set r */
    BN_one(bn_s);  /* set s */
    BN_one(bn_two_pow_s);  /* set 2^s */
    BN_mul(bn_two_pow_s_mul_r, bn_two_pow_s, bn_r, bn_ctx);  /* set 2^s*r */

    while (BN_cmp(bn_two_pow_s_mul_r, bn_n_minus_one) <= 0) {
        BN_mul(bn_two_pow_s, bn_two_pow_s, bn_two, bn_ctx);
        BN_div(bn_r, NULL, bn_n_minus_one, bn_two_pow_s, bn_ctx);
        if (BN_is_odd(bn_r)) {
            flag = 1;
            break;
        }
        else {
            BN_add(bn_s, bn_s, bn_one);
            BN_mul(bn_two_pow_s_mul_r, bn_two_pow_s, bn_r, bn_ctx);
        }
    }

    const char* str_bn_n = BN_bn2dec(bn_num);
    const char* str_bn_n_minus_one = BN_bn2dec(bn_n_minus_one);
    const char* str_bn_s = BN_bn2dec(bn_s);
    const char* str_bn_r = BN_bn2dec(bn_r);

    if (nIndent == 0) {
        printf("%sn = %s\n", strIndent, str_bn_n);
    }
    printf("%s  n-1 = %s\n", strIndent, str_bn_n_minus_one);
    printf("%s  s = %s\n", strIndent, str_bn_s);
    printf("%s  r = %s\n", strIndent, str_bn_r);

    BN_sub(bn_s_minus_one, bn_s, bn_one);  /* set s-1 */

    int nObj = fread(&buf, 4, 1, fp);  /* don't need `maxval' */

    for (uint32_t i = 1; i <= maxitr; ++i) {
        if (feof(fp)) {
            fprintf(stderr, "Error: reached end of primesfile, bailing.\n");
            exit(-1);
        }
        nObj = fread(&buf, 4, 1, fp);
        if (nObj == 0) {
            fprintf(stderr, "Error: reached end of primesfile, bailing.\n");
            exit(-1);
        }
        /* 2.1) */
        if (!BN_bin2bn((uint8_t*)(&buf), sizeof(uint32_t), bn_a)) goto end;
        const char* str_bn_a = BN_bn2dec(bn_a);
        /* 2.1.1) */
        if (BN_cmp(bn_a, bn_n_minus_one) == 1) {
            fprintf(stderr, "Error: either `maxitr' too large or not enough primes in `primesfile', bailing.\n");

            return -2;
        }
        /* 2.2) */
        if (!BN_mod_exp(bn_y, bn_a, bn_r, bn_num, bn_ctx)) goto end;
        const char* str_bn_y = BN_bn2dec(bn_y);
        if (!strcmp(str_bn_y, str_bn_n_minus_one))
            printf("%s  Itr %u of %llu, a = %s, y = %s (which is n-1)\n", strIndent, i, maxitr, str_bn_a, str_bn_y);
        else
            printf("%s  Itr %u of %llu, a = %s, y = %s\n", strIndent, i, maxitr, str_bn_a, str_bn_y);
        /* 2.3) */
        if (!BN_is_one(bn_y) && BN_cmp(bn_y, bn_n_minus_one) != 0) {
            /* 2.3.1) */
            for (BN_one(bn_j); BN_cmp(bn_j, bn_s_minus_one) <= 0 &&
                    BN_cmp(bn_y, bn_n_minus_one) != 0; BN_add(bn_j, bn_j, bn_one)) {
                /* 2.3.1.1) */
                if (!BN_mod_sqr(bn_y_sqr, bn_y, bn_num, bn_ctx)) goto end;
                if (!BN_copy(bn_y, bn_y_sqr)) goto end;
                const char* str_bn_j = BN_bn2dec(bn_j);
                const char* str_bn_s_minus_one = BN_bn2dec(bn_s_minus_one);
                str_bn_y = BN_bn2dec(bn_y);
                if (!strcmp(str_bn_y, str_bn_n_minus_one))
                    printf("%s    j = %s of %s, y = %s (which is n-1)\n", strIndent, str_bn_j, str_bn_s_minus_one, str_bn_y);
                else
                    printf("%s    j = %s of %s, y = %s\n", strIndent, str_bn_j, str_bn_s_minus_one, str_bn_y);
                /* 2.3.1.2) */
                if (!strcmp(str_bn_y, "1")) {
                    printf("%sMiller-Rabin found a strong witness %s\n", strIndent, str_bn_a);

                    return -1;
                }
            }
            /* 2.3.2) */
            if (BN_cmp(bn_y, bn_n_minus_one) != 0) {
                printf("%sMiller-Rabin found a strong witness %s\n", strIndent, str_bn_a);

                return -1;
            }
        }
    }

end:

    error = ERR_get_error();

    if (error) {
        const char* strError = ERR_error_string(error, NULL);
        fprintf(stderr, "%s\n", strError);
    }
    else {
        printf("%sMiller-Rabin declares n to be a prime number\n", strIndent);
    }

    return 0;
}

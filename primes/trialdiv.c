
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "trialdiv.h"


int trialdiv(BIGNUM* bn_num, FILE* fp, int flagPrint)
{
    rewind(fp);

    BIGNUM* bn_m = NULL;
    BIGNUM* bn_rem = NULL;
    BIGNUM* bn_sqr = NULL;
    BIGNUM* bn_maxval = NULL;

    bn_m = BN_new();
    bn_rem = BN_new();
    bn_sqr = BN_new();
    bn_maxval = BN_new();

    if (!bn_m || !bn_rem || !bn_sqr || !bn_maxval) {
        fprintf(stderr, "Error: couldn't alloc BIGNUM, bailing.\n");
        exit(-1);
    }

    BN_CTX* bn_ctx = NULL;

    bn_ctx = BN_CTX_new();

    if (!bn_ctx) {
        fprintf(stderr, "Error: couldn't alloc BN_CTX, bailing.\n");
        exit(-1);
    }

    BN_zero(bn_m);
    BN_zero(bn_rem);
    BN_zero(bn_sqr);
    BN_zero(bn_maxval);

    BN_CTX_init(bn_ctx);

    int flag = 0;
    int nObj = 0;
    uint32_t buf = 0;

    fread(&buf, 4, 1, fp);

    if (!BN_bin2bn((uint8_t*)(&buf), sizeof(uint32_t), bn_maxval)) {
        fprintf(stderr, "Error: couldn't convert BIGNUM, bailing.\n");
        exit(-1);
    }

    while (!feof(fp)) {
        nObj = 0;
        buf = 0;

        BN_zero(bn_m);
        BN_zero(bn_rem);
        BN_zero(bn_sqr);

        nObj = fread(&buf, 4, 1, fp);

        if (nObj == 0) {
            break;
        }

        if (!BN_bin2bn((uint8_t*)(&buf), sizeof(uint32_t), bn_m)) {
            fprintf(stderr, "Error: couldn't convert BIGNUM, bailing.\n");
            exit(-1);
        }

        if (!BN_sqr(bn_sqr, bn_m, bn_ctx)) {
            fprintf(stderr, "Error: couldn't perform BIGNUM op, bailing.\n");
            exit(-1);
        }

        if (BN_cmp(bn_sqr, bn_num) == 1) {
            break;  /* we've gone past the sqrt of bn_num */
        }

        if (!BN_mod(bn_rem, bn_num, bn_m, bn_ctx)) {
            fprintf(stderr, "Error: couldn't perform BIGNUM op, bailing.\n");
            exit(-1);
        }

        if (BN_is_zero(bn_rem)) {
            flag = 1;
            break;
        }
    }

    BN_zero(bn_sqr);

    if (!BN_sqr(bn_sqr, bn_maxval, bn_ctx)) {
        fprintf(stderr, "Error: couldn't perform BIGNUM op, bailing.\n");
        exit(-1);
    }

    if (flag) {
        if (flagPrint) {
            printf("n is composite by trial division (mod %s = 0)\n", BN_bn2dec(bn_m));
        }

        return atoi(BN_bn2dec(bn_m));
    }
    else if (BN_cmp(bn_sqr, bn_num) == -1) {
        if (flagPrint) {
            printf("n passes trial division test (not enough primes)\n");
        }

        return -2;
    }

    if (flagPrint) {
        printf("n passes trial division test\n");
    }

    return 0;
}

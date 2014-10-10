
#include <stdlib.h>
#include <openssl/err.h>

#include "rndsearch.h"
#include "trialdiv.h"
#include "millerrabin.h"
#include "util.h"


BIGNUM* rndsearch(int k, int t, FILE* fpPrimes, FILE* fpRnd)
{
    int m = 0;
    int iter = 1;
    uint32_t error = 0;
    BIGNUM* bn_n = NULL;

    if (!(bn_n = BN_new())) goto end;

step1:

    printf("RANDOM-SEARCH: iteration %d\n", iter++);

    rndOddNum(k, fpRnd, bn_n);

    printf("  n = %s\n", BN_bn2dec(bn_n));

    if ((m = trialdiv(bn_n, fpPrimes, 0)) > 0) {
        printf("  n is composite by trial division (mod %d = 0)\n", m);
        goto step1;
    }
    else {
        printf("  n passes trial division test\n");
    }

    if (millerrabin(bn_n, t, fpPrimes, 2) == 0) {
        goto end;
    }

    goto step1;

end:

    error = ERR_get_error();

    if (error) {
        const char* strError = ERR_error_string(error, NULL);
        fprintf(stderr, "%s\n", strError);
        exit(-1);
    }

    return bn_n;
}

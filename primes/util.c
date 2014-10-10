
#include <stdint.h>
#include <math.h>
#include <openssl/err.h>

#include "util.h"


void rndOddNum(int k, FILE* fp, BIGNUM* bn_res)
{
    uint32_t error = 0;
    uint8_t buf[CHAR_BUF_LEN];

    int x = ceil(k/8.0);

    if (x > CHAR_BUF_LEN) {
        fprintf(stderr, "Error: %d > CHAR_BUF_LEN in rndOddNum, bailing.\n", x);
        exit(-1);
    }

    if (feof(fp)) {
        fprintf(stderr, "Error: feof on `fp' in rndOddNum, bailing.\n");
        exit(-1);
    }

    int nObj = fread(buf, 1, x, fp);

    if (nObj != x) {
        fprintf(stderr, "Error: only read %d of %d bytes in rndOddNum, bailing.\n", nObj, x);
        exit(-1);
    }

    if (!BN_bin2bn((uint8_t*)(&buf), x, bn_res)) goto end;

    int nBits = BN_num_bits(bn_res);

    for (int i = nBits-1; i >= k; --i) {
        BN_clear_bit(bn_res, i);
    }

    BN_set_bit(bn_res, k-1);
    BN_set_bit(bn_res, 0);

end:

    error = ERR_get_error();

    if (error) {
        const char* strError = ERR_error_string(error, NULL);
        fprintf(stderr, "%s\n", strError);

        return;
    }
}

uint8_t rndByte(FILE* fp)
{
    uint8_t buf = 0;

    if (feof(fp)) {
        fprintf(stderr, "Error: feof on `fp' in rndByte, bailing.\n");
        exit(-1);
    }

    int nObj = fread(&buf, 1, 1, fp);

    if (nObj != 1) {
        fprintf(stderr, "Error: couldn't read byte from `fp' in rndByte, bailing.\n");
        exit(-1);
    }

    return buf;
}

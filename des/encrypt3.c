
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "encrypt3.h"
#include "tablecheck.h"
#include "keyschedule.h"
#include "des.h"
#include "util.h"


/*
 * Triple DES encryption of input from FILE pointer `fp' using key `sKey'
 * and tables specified in file `tablefile'.
 */
void encrypt3(const char* sKey, const char* tablefile, FILE* fp)
{
    if (strlen(sKey) != 48) {
        fprintf(stderr, "Error: invalid key length, bailing.\n");
        exit(-1);
    }

    char ssKey[3][16+1];

    memset(ssKey[0], '\0', (16+1) * sizeof(char));
    memset(ssKey[1], '\0', (16+1) * sizeof(char));
    memset(ssKey[2], '\0', (16+1) * sizeof(char));

    memcpy(ssKey[0], &sKey[0], 16 * sizeof(char));
    memcpy(ssKey[1], &sKey[16], 16 * sizeof(char));
    memcpy(ssKey[2], &sKey[32], 16 * sizeof(char));

    uint8_t buf[8];  /* block size is 64 bits */
    memset(buf, 0, 8 * sizeof(uint8_t));

    int keys[3][16][48];  /* represent each key bit as an integer */
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 16; ++j) {
            memset(keys[i][j], 0, 48 * sizeof(int));
        }
    }

    int** table = tablecheck(tablefile);

    int* V = table[11];
    int* PC1 = table[12];
    int* PC2 = table[13];

    for (int i = 0; i < 3; ++i) {
        keyschedule(ssKey[i], V, PC1, PC2, keys[i]);
    }

    /* reverse second key for EDE mode */
    reverse(keys[1], 16);

    int m[64];
    int res[64];

    memset(m, 0, 64 * sizeof(int));
    memset(res, 0, 64 * sizeof(int));

    while (!feof(fp)) {
        memset(buf, 0, 8 * sizeof(uint8_t));
        int nObj = 0;
        for (int i =0; i < 8; ++i) {
            nObj = fread(&buf[i], 1, 1, fp);
            if (nObj == 0) {
                if (i == 0) {
                    return;
                }
                else {
                    break;
                }
            }
        }

        unpack(buf, m);

        des(m, table, keys[0], res);

        memcpy(m, res, 64 * sizeof(int));
        memset(res, 0, 64 * sizeof(int));

        des(m, table, keys[1], res);

        memcpy(m, res, 64 * sizeof(int));
        memset(res, 0, 64 * sizeof(int));

        des(m, table, keys[2], res);

        pack(res, buf);

        fwrite(buf, 1, 8, stdout);
    }
}

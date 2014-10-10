
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "encrypt.h"
#include "tablecheck.h"
#include "keyschedule.h"
#include "des.h"
#include "util.h"


/*
 * DES encryption of input from FILE pointer `fp' using key `sKey' and
 * tables specified in file `tablefile'.
 */
void encryptdes(const char* sKey, const char* tablefile, FILE* fp)
{
    uint8_t buf[8];  /* block size is 64 bits */
    memset(buf, 0, 8 * sizeof(uint8_t));

    int keys[16][48];  /* represent each key bit as an integer */
    for (int i = 0; i < 16; ++i) {
        memset(keys[i], 0, 48 * sizeof(int));
    }

    int** table = tablecheck(tablefile);

    int* V = table[11];
    int* PC1 = table[12];
    int* PC2 = table[13];

    keyschedule(sKey, V, PC1, PC2, keys);

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
        des(m, table, keys, res);
        pack(res, buf);

        fwrite(buf, 1, 8, stdout);
    }
}

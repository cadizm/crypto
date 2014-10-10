
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "enc-base64.h"
#include "dec-base64.h"


#define BUF_LEN       4
#define JOIN_LEN      4

static uint8_t getBase64Index(uint8_t c);


/**
 * Perform a base64 decoding of file `infile' or stdin in file infile is
 * passed to this function as NULL. Output is written to stdout and errors
 * to stderr. Returns non-zero integer on error, zero on success, and exits
 * if an invalid character is encountered during processing.
 */
int base64decode(const char* infile)
{
    int nStatus = 0;
    int nCount = 0;
    FILE* fp = NULL;
    uint8_t cur = '\0';
    uint8_t buf[BUF_LEN];

    memset(buf, '\0', BUF_LEN * sizeof(uint8_t));

    if (infile == NULL) {
        fp = stdin;
    }

    else {
        if (!(fp = fopen(infile, "rb"))) {
            char errorMsg[255];
            sprintf(errorMsg, "Error opening %s", infile);
            perror(errorMsg);
            exit(-1);
        }
    }

    while (!feof(fp)) {
        int nObj = 0;

        nObj = fread(&cur, 1, 1, fp);
        nStatus |= ferror(fp);

        if (cur == '\r' || cur == '\n') {
            continue;
        }
        else {
            buf[nCount++] = cur;
        }

        if (nCount == JOIN_LEN) {
            nCount = 0;
            int temp = 0;

            uint8_t index0 = getBase64Index(buf[0]);
            uint8_t index1 = getBase64Index(buf[1]);
            uint8_t index2 = getBase64Index(buf[2]);
            uint8_t index3 = getBase64Index(buf[3]);

            memset(buf, '\0', BUF_LEN * sizeof(uint8_t));

            index0 <<= 2;
            temp = index1 & 0x30;
            temp >>= 4;
            int pack0 = index0 | temp;
            printf("%c", pack0);

            if (index2 == PAD) {
                break;
            }
            index1 &= 0x0f;
            index1 <<= 4;
            temp = index2 & 0x3c;
            temp >>= 2;
            int pack1 = index1 | temp;
            printf("%c", pack1);

            if (index3 == PAD) {
                break;
            }
            index2 &= 0x03;
            index2 <<= 6;
            int pack2 = index2 | index3;
            printf("%c", pack2);
        }
    }

    if (nCount != 0) {
        fprintf(stderr, "Error: bad input, halting.\n");
        exit(-1);
    }

    return nStatus;
}

/**
 * Helper function to return the base64 `index' of the passed character `c',
 * where index is the zero-based index of accepted base64-encoded characters
 * defined in enc-base64.h. Exits program if the passed character is invalid.
 */
uint8_t getBase64Index(uint8_t c)
{
    int i;

    for (i = 0; i < TABLE_LEN; ++i) {
        if (BASE64_ENCODING_TABLE[i] == c) {
            return i;
        }
    }

    fprintf(stderr, "Error: bad input, halting.\n");
    exit(-1);
}

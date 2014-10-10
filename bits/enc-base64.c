
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "enc-base64.h"


#define BUF_LEN    3
#define WRAP_LEN  64

static int m_curcol = 0;

static void printEnc64(int nObj, uint8_t* buf);
static void wrap();


/**
 * Base64 encode file `infile' or stdin if infile is passed as NULL. Output is
 * written to stdout and errors to stderr. Returns non-zero integer on error,
 * zero on success.
 */
int base64encode(const char* infile)
{
    int nStatus = 0;
    FILE* fp = NULL;
    uint8_t buf[BUF_LEN];

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
        memset(buf, '\0', BUF_LEN * sizeof(uint8_t));

        nObj = fread(buf, 1, BUF_LEN, fp);
        nStatus |= ferror(fp);

        printEnc64(nObj, buf);
    }

    if (m_curcol % WRAP_LEN != 0) {
        printf("\n");
    }

    return nStatus;
}

/**
 * Print the base64 encoded string for `buf', given `nObj' were last read
 * from the input stream.
 */
void printEnc64(int nObj, uint8_t* buf)
{
    uint32_t jbuf = 0;
    uint8_t pack1 = 0;
    uint8_t pack2 = 0;
    uint8_t pack3 = 0;
    uint8_t pack4 = 0;

    if (nObj == 0) {
        return;
    }

    jbuf |= buf[0];
    jbuf <<= 8;
    jbuf |= buf[1];
    jbuf <<= 8;
    jbuf |= buf[2];

    pack1 = jbuf >> 18;

    pack2 = jbuf >> 12;
    pack2 &= 0x3f;

    pack3 = jbuf >> 6;
    pack3 &= 0x3f;

    pack4 = jbuf & 0x3f;

    if (nObj == 1) {
        pack3 = PAD;
        pack4 = PAD;
    }
    else if (nObj == 2) {
        pack4 = PAD;
    }

    printf("%c", BASE64_ENCODING_TABLE[pack1]); wrap();
    printf("%c", BASE64_ENCODING_TABLE[pack2]); wrap();
    printf("%c", BASE64_ENCODING_TABLE[pack3]); wrap();
    printf("%c", BASE64_ENCODING_TABLE[pack4]); wrap();
}

/**
 * Print a newline if output is at column `WRAP_LEN'.
 */
void wrap()
{
    m_curcol++;

    if (m_curcol % WRAP_LEN == 0) {
        printf("\n");
    }
}

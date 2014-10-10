
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hexdump.h"


#define BUF_LEN  16

static void dumpline(int nByteAddr, int nObj, const uint8_t* buf);


/**
 * Hexdump file `infile' or stdin if infile is passed as NULL. Output is
 * written to stdout and errors to stderr. Returns non-zero integer on error,
 * zero on success.
 */
int hexdump(const char* infile)
{
    int nStatus = 0;
    FILE* fp = NULL;
    uint8_t buf[BUF_LEN];
    int nByteAddr = 0;

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

        if (nObj == 0) {
            break;
        }
        dumpline(nByteAddr, nObj, buf);
        nByteAddr += BUF_LEN;
    }

    if (infile != NULL && fp) {
        nStatus |= fclose(fp);
    }

    return nStatus;
}

/**
 * Dump line `buf' to stdout given `nObj' objects were last read from input.
 * `nByteAddr' is the start address of the input line.
 */
void dumpline(int nByteAddr, int nObj, const uint8_t* buf)
{
    int i = 0;

    printf("%06x:", nByteAddr);

    for (i = 0; i < BUF_LEN; ++i) {
        if (i >= nObj) {
            printf(" --");
        }
        else {
            printf(" %02x", buf[i]);
        }

        if (i == 7) {
            printf(" ");
        }
    }

    printf("  ");

    for (i = 0; i < BUF_LEN; ++i) {
        if (i >= nObj) {
            printf(" ");
        }
        else if (buf[i] <= 0x7e && buf[i] >= 0x20) {
            printf("%c", buf[i]);
        }
        else if (buf[i] <= 0x1f || buf[i] == 0x7f) {
            printf(".");
        }
        else if (buf[i] >= 0x80) {
            printf("~");
        }
    }

    printf("\n");
}

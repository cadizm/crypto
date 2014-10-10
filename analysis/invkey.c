
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "invkey.h"

#define ALPHABET_LEN  26


/*
 * Produce an inverse key (suitable for use in a polyalphabetic cipher) for
 * the passed key file `keyfile'.
 */
void invkey(const char* keyfile)
{
    FILE* fp = NULL;

    if (!(fp = fopen(keyfile, "r"))) {
        char errorMsg[CHAR_BUF_LEN];
        sprintf(errorMsg, "Error opening %s", keyfile);
        perror(errorMsg);
        exit(-1);
    }

    char buf[ALPHABET_LEN];
    char valbuf[ALPHABET_LEN];

    memset(buf, '\0', ALPHABET_LEN);
    memset(valbuf, 0, ALPHABET_LEN);

    int k = 0;
    int c = EOF;

    while ((c = fgetc(fp)) != EOF) {
        if ((c != 0x0a && c < 0x61) || c > 0x7a) {
            fprintf(stderr, "Bad keyfile, invalid character: 0x%x\n", c);
            exit(-1);
        }

        if (c == 0x0a) {
            for (int i = 0; i < ALPHABET_LEN; ++i) {
                if (valbuf[i] != 1) {
                    fprintf(stderr, "Bad keyfile, check input\n");
                    exit(-1);
                }
            }

            for (int i = 0; i < ALPHABET_LEN; ++i) {
                for (int j = 0; j < ALPHABET_LEN; ++j) {
                    if (buf[j] == i + 'a') {
                        printf("%c", j + 'a');
                    }
                }
            }

            k = 0;
            memset(buf, '\0', ALPHABET_LEN);
            memset(valbuf, 0, ALPHABET_LEN);

            printf("\n");
            continue;
        }
        else {
            buf[k++] = c;
            valbuf[c-'a'] += 1;
        }
    }
}

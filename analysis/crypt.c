
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "crypt.h"

#define ALPHABET_LEN  26


/*
 * Encrypt fpInput using a Vigenere cipher and the key specified in
 * file keyfile.
 */
void crypt_(const char* keyfile, FILE* fpInput)
{
    FILE* fpKey = NULL;
    int nLines = 0;
    int k = 0;
    int c = EOF;
    char buf[ALPHABET_LEN+2];  /* add 2: 1 for '\n', 1 for '\0' */

    if (!(fpKey = fopen(keyfile, "r"))) {
        char errorMsg[CHAR_BUF_LEN];
        sprintf(errorMsg, "Error opening %s", keyfile);
        perror(errorMsg);
        exit(-1);
    }

    while ((c = fgetc(fpKey)) != EOF) {
        if ((c != 0x0a && c < 0x20) || c > 0x7e) {
            fprintf(stderr, "Bad keyfile, invalid character: 0x%x\n", c);
            exit(-1);
        }
    }

    rewind(fpKey);

    while (!feof(fpKey)) {
        if (fgets(buf, ALPHABET_LEN+2, fpKey) != NULL) {
            nLines++;
        }
    }

    char** keyBuf = malloc(nLines * ALPHABET_LEN);
    if (!keyBuf) {
        fprintf(stderr, "Error allocating keyBuf, bailing\n");
        exit(-1);
    }
    memset(keyBuf, '\0', nLines * ALPHABET_LEN);

    for (int i = 0; i < nLines; ++i) {
        keyBuf[i] = malloc(ALPHABET_LEN);
        if (!keyBuf[i]) {
            fprintf(stderr, "Error allocating keyBuf[%d], bailing\n", i);
            exit(-1);
        }
        memset(keyBuf[i], '\0', ALPHABET_LEN);
    }

    rewind(fpKey);

    char alphabuf[ALPHABET_LEN];
    memset(alphabuf, 0, ALPHABET_LEN);

    for (int i = 0; i < nLines; ++i) {
        if (fgets(buf, ALPHABET_LEN+2, fpKey) != NULL) {
            memcpy(keyBuf[i], buf, ALPHABET_LEN);
            memset(alphabuf, 0, ALPHABET_LEN);
        }
        for (int j = 0; j < ALPHABET_LEN; ++j) {
            if (keyBuf[i][j] > 0x60 && keyBuf[i][j] < 0x7b) {
                char c = keyBuf[i][j];
                alphabuf[c-'a'] += 1;
            }
        }
        for (int j = 0; j < ALPHABET_LEN; ++j) {
            if (alphabuf[j] != 1) {
                fprintf(stderr, "Error: invalid keyfile, bailing\n");
                exit(-1);
            }
        }
    }

    if (fpKey) {
        fclose(fpKey);
    }

    while ((c = fgetc(fpInput)) != EOF) {
        if (c < 0x61 || c > 0x7a) {
            printf("%c",c);
        }
        else {
            char* key = keyBuf[k % nLines];
            printf("%c", key[c-'a']);
        }
        k += 1;
    }
}

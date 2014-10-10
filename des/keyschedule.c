
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "keyschedule.h"
#include "util.h"


static void lcs(int buf[28], int n);

/*
 * DES Key scheduling using key `sKey' and tables `V', `PC1', and `PC2'.  The
 * output round subkeys are stored in arg `keys'.
 */
void keyschedule(const char* sKey, int V[16], int PC1[56], int PC2[48], int keys[16][48])
{
    int k = 0;
    int key[64];
    char buf[2];

    memset(key, 0, 64 * sizeof(int));
    memset(buf, 0, 2 * sizeof(char));

    if (strlen(sKey) != 16) {
        fprintf(stderr, "Error: invalid key length, bailing.\n");
        exit(-1);
    }

    /* convert to internal representation */
    for (int i = 0; i < 16; ++i) {
        buf[0] = tolower(sKey[i]);
        if (!(buf[0] > 0x60 && buf[0] < 0x67) &&
            !(buf[0] > 0x2f && buf[0] < 0x3a)) {
            fprintf(stderr, "Error: invalid key char `%c', bailing.\n", buf[0]);
            exit(-1);
        }

        uint32_t byte = 0;
        sscanf(buf, "%x", &byte);
        key[k++] = 0x08 & byte ? 1 : 0;
        key[k++] = 0x04 & byte ? 1 : 0;
        key[k++] = 0x02 & byte ? 1 : 0;
        key[k++] = 0x01 & byte ? 1 : 0;
    }

    int C[28];
    int D[28];

    memset(C, 0, 28 * sizeof(int));
    memset(D, 0, 28 * sizeof(int));

    /* setup C and D (aka `T') using PC1 */
    for (int i = 0; i < 28; ++i) {
        int pos = PC1[i] - 1;  /* decrement by 1, DES tables are 1-based */
        C[i] = key[pos];
    }
    for (int i = 0; i < 28; ++i) {
        int pos = PC1[28+i] - 1;  /* decrement by 1, DES tables are 1-based */
        D[i] = key[pos];
    }

    int CD[56];
    memset(CD, 0, 56 * sizeof(int));

    k = 0;

    /* concatenate C and D */
    for (int j = 0; j < 28; ++j) {
        CD[k++] = C[j];
    }
    for (int j = 0; j < 28; ++j) {
        CD[k++] = D[j];
    }

    uint8_t cdstr[56/4+1];
    memset(cdstr, '\0', sizeof(cdstr));

    hexstring(CD, 56, cdstr);
    fprintf(stderr, "(C0,D0)=%s\n", cdstr);

    uint8_t keystr[48/4+1];
    memset(keystr, '\0', sizeof(keystr));

    /* Compute each key, keys[i], for use during each round */
    for (int i = 0; i < 16; ++i) {

        /* left circular shift by v */
        lcs(C, V[i]);
        lcs(D, V[i]);

        k = 0;

        /* concatenate C and D */
        memcpy(&CD[0], C, 28 * sizeof(int));
        memcpy(&CD[28], D, 28 * sizeof(int));

        memset(cdstr, '\0', sizeof(cdstr));
        hexstring(CD, 56, cdstr);
        fprintf(stderr, "(C%d,D%d)=%s\n", i+1, i+1, cdstr);

        k = 0;

        for (int j = 0; j < 24; ++j) {
            int pos = PC2[j] - 1;  /* decrement by 1, DES tables are 1-based */
            keys[i][k++] = CD[pos];
        }
        for (int j = 0; j < 24; ++j) {
            int pos = PC2[24+j] - 1;  /* decrement by 1, DES tables are 1-based */
            keys[i][k++] = CD[pos];
        }

        memset(keystr, '\0', sizeof(keystr));
        hexstring(keys[i], 48, keystr);
        fprintf(stderr, "k%d=%s\n", i+1, keystr);
    }
}

/*
 * Left circular shift of buf `n' places.
 */
void lcs(int buf[28], int n)
{
    n = n % 28;

    int* temp = malloc(n * sizeof(int));
    if (!temp) {
        fprintf(stderr, "Error: couldn't alloc temp buffer, bailing.\n");
        exit(-1);
    }

    for (int i = 0; i < n; ++i) {
        temp[i] = buf[i];
    }
    for (int i = n; i < 28; ++i) {
        buf[i-n] = buf[i];
    }
    for (int i = 0; i < n; ++i) {
        buf[28-n+i] = temp[i];
    }

    if (temp) {
        free(temp);
    }
}

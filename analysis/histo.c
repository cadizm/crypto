
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "histo.h"

#define ALPHABET_LEN  26

typedef struct {
    char letter;
    int occ;
} letterOcc_t;


static int compareLetterOcc_t(const void* a, const void* b);

/*
 * Produce a histogram of letter frequences assuming period `period' for
 * the 1-based column `col' for the given filehandle `fp'.
 */
void histo(int period, int col, FILE* fp)
{
    int i = 1;
    int j = 1;
    int c = EOF;
    int L = 0;

    int occBuf[ALPHABET_LEN];
    memset(occBuf, 0, ALPHABET_LEN * sizeof(int));

    while ((c = fgetc(fp)) != EOF) {
        if (j++ == col && c >= 0x61 && c <= 0x7a) {
            occBuf[c-'a'] += 1;
        }
        if (i++ % period == 0) {
            j = 1;
        }
    }

    for (int i = 0; i < ALPHABET_LEN; ++i) {
        L += occBuf[i];
    }

    letterOcc_t letterOcc[ALPHABET_LEN];
    memset(letterOcc, 0, ALPHABET_LEN * sizeof(letterOcc_t));

    for (int i = 0; i < ALPHABET_LEN; ++i) {
        letterOcc[i].letter = i + 'a';
        letterOcc[i].occ = occBuf[i];
    }

    qsort(letterOcc, ALPHABET_LEN, sizeof(letterOcc_t), compareLetterOcc_t);

    printf("L=%d\n", L);

    for (int i = 0; i < ALPHABET_LEN; ++i) {
        printf("%c: %d (%2.2f%%)\n", letterOcc[i].letter, letterOcc[i].occ,
                letterOcc[i].occ / ((float)L) * 100);
    }
}

/*
 * Sort descending
 */
int compareLetterOcc_t(const void* a, const void* b)
{
    letterOcc_t* aa = (letterOcc_t*)a;
    letterOcc_t* bb = (letterOcc_t*)b;

    if (aa->occ > bb->occ) {
        return -1;
    }
    else if (aa->occ < bb->occ) {
        return 1;
    }

    return 0;
}

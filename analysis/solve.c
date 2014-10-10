
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "solve.h"

#define MIN_SEARCH_LEN  4
#define ALPHABET_LEN  26
#define Kp  0.0658
#define Kr  1.0 / ALPHABET_LEN


static void kasiski(char* input);
static int findstr(char* haystack, char* needle, int start);
static void indexOfCoincidence(char* input, int max_t);
static void autoCorrelation(char* input, int max_t);

/*
 * `Solve' using the Kasiski method, Index of Coincident and Auto-correlation
 * methods the input specified in file pointer `fp' assuming a maximum period
 * of `max_t'.
 */
void solve(int max_t, FILE* fp)
{
    int c = EOF;
    int size = 0;
    int pos = 0;

    while ((c = fgetc(fp)) != EOF) {
        size += 1;
    }

    char* input = malloc((size+1) * sizeof(char));
    memset(input, '\0', (size+1) * sizeof(char));

    rewind(fp);

    while ((c = fgetc(fp)) != EOF) {
        input[pos++] = c;
    }

    kasiski(input);
    printf("\n");
    indexOfCoincidence(input, max_t);
    printf("\n");
    autoCorrelation(input, max_t);
    printf("\n");
}

void kasiski(char* input)
{
    printf("Kasiski Method\n==============\n");

    int maxSearchLen = strlen(input) / 2;
    char buf[maxSearchLen+1];
    memset(buf, '\0', sizeof(buf));

    for (int len = MIN_SEARCH_LEN; len <= maxSearchLen+1; ++len) {
        int matches = 0;

        for (int pos = 0; pos < strlen(input) - len; ++pos) {
            int bpos = 0;

            for (int i = pos; i < pos + len && isalpha(input[i]); ++i) {
                buf[bpos++] = input[i];
            }

            if (strlen(buf) < len) {
                continue;
            }

            matches += findstr(input, buf, pos + strlen(buf));
            memset(buf, '\0', sizeof(buf));
        }
        if (matches == 0) {
            printf("len=%d, no more matches\n", len);
            break;
        }
    }
}

/*
 * Search for `needle' starting at `start' in `haystack' and print matches.
 */
int findstr(char* haystack, char* needle, int start)
{
    int j = 0;
    int matches = 0;
    int len = strlen(needle);

    for (int i = start; i < strlen(haystack) + len; ++i) {
        for (j = 0; j < len; ++j) {
            if (haystack[i+j] != needle[j]) {
                break;
            }
        }

        if (j == len) {
            matches += 1;
            printf("len=%d, i=%d, j=%d, j-i=%d, %s\n",
                    len, start-len, i+j-len, i+j-start, needle);
        }
    }

    return matches;
}

void indexOfCoincidence(char* input, int max_t)
{
    printf("Average Index of Coincidence\n============================\n");

    int alphaCount[ALPHABET_LEN];
    memset(alphaCount, 0, ALPHABET_LEN * sizeof(int));

    for (int i = 0; i < strlen(input); ++i) {
        if (input[i] > 0x60 && input[i] < 0x7b) {
            int pos = input[i] - 'a';
            alphaCount[pos] += 1;
        }
    }

    int L = 0;

    for (int i = 0; i < ALPHABET_LEN; ++i) {
        L += alphaCount[i];
    }

    printf("L=%d\n", L);

    for (int i = 0; i < ALPHABET_LEN; ++i) {
        printf("f('%c')=%d\n", i+'a', alphaCount[i]);
    }

    int s = 0;

    for (int i = 0; i < ALPHABET_LEN; ++i) {
        s += alphaCount[i] * (alphaCount[i] - 1);
    }

    double IC = ((double)s) / ((double)(L * (L - 1)));

    printf("IC=%.8lg\n", IC);

    for (int t = 1; t <= max_t; ++t) {
        double EIC = (1.0 / t * (L - t) / (L - 1) * Kp) +
                ((t - 1.0) / t) * L / (L - 1.0) * Kr;
        printf("t=%d, E(IC)=%.8lg\n", t, EIC);
    }
}

void autoCorrelation(char* input, int max_t)
{
    printf("Auto-correlation Method\n=======================\n");

    for (int t = 1; t <= max_t; ++t) {
        int count = 0;
        for (int i = 0; i < strlen(input) - t; ++i) {
            if (input[i] > 0x60 && input[i] < 0x7b && input[i] == input[i+t]) {
                count += 1;
            }
        }

        printf("t=%d, count=%d\n", t, count);
    }
}

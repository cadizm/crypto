
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "keygen.h"

#define INTERNAL_STATE_LEN  26
#define KEYLENGTH  sizeof(time_t)


static void swap(int* a, int* b);


/*
 * Generate a keyfile suitable for use in a polyalphabetic cipher using the
 * specified period `period'.
 *
 * Implementation note: this function uses a modified version of the RC4
 * key-scheduling algorithm's internal state as the key output.
 */
void keygen(int period)
{
    srand(time(NULL));

    time_t now = time(NULL);
    int8_t* key = (int8_t*)&now;
    int S[INTERNAL_STATE_LEN];

    for (int i = 0; i < INTERNAL_STATE_LEN; ++i) {
        S[i] = i;
    }

    for (int k = 0; k < period; ++k) {
        for (int i = 0, j = 0; i < INTERNAL_STATE_LEN; ++i) {
            j = (j + S[i] + ((key[i % KEYLENGTH]) & 0xff)) % INTERNAL_STATE_LEN;
            swap(&S[i], &S[j]);
        }

        for (int i = 0, j = 0; i < pow(10, INTERNAL_STATE_LEN * rand()); ++i) {
            i = (i + 1) % INTERNAL_STATE_LEN;
            j = (j + (S[i] & 0xff)) % INTERNAL_STATE_LEN;
            swap(&S[i], &S[j]);
        }

        for (int i = 0; i < INTERNAL_STATE_LEN; ++i) {
            printf("%c", S[i] + 'a');
        }

        printf("\n");
    }
}

void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

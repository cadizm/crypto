
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "primes.h"


static void eratosthenes(uint32_t maxval, uint32_t* a);


void primes(uint32_t maxval)
{
    uint32_t* a = malloc((maxval+1) * sizeof(uint32_t));
    memset(a, 0, (maxval+1) * sizeof(uint32_t));

    eratosthenes(maxval, a);

    uint32_t buf = 0;

    buf = htonl(maxval);
    fwrite(&buf, 4, 1, stdout);

    for (int i = 0; i <= maxval; ++i) {
        if (a[i]) {
            buf = htonl(i);
            fwrite(&buf, 4, 1, stdout);
        }
    }
}

/*
 * `maxval' is 2^24, inclusive.
 * Array `a' should be of size maxval+1
 */
void eratosthenes(uint32_t maxval, uint32_t* a)
{
    a[1] = 0;

    for (int i = 2; i <= maxval; ++i) {
        a[i] = 1;
    }

    int p = 2;

    while ((p << 1) <= maxval) {
        int j = p << 1;
        while (j <= maxval) {
            a[j] = 0;
            j += p;
        }
        do {
            p += 1;
        } while (a[p] != 1);
    }
}

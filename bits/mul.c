
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "mul.h"


#define BUF_LEN   15
#define SENTINEL  -1
#define RADIX     10

typedef enum {
    MULTIPLIER    = 0x01,
    MULTIPLICAND  = 0x02,
    PARTIAL       = 0x04,
    PRODUCT       = 0x08,
    DASH          = 0x10
} mtype;


static void pprint(uint64_t n1, uint64_t n2,
                   uint64_t res, uint64_t buf[BUF_LEN]);
static void pprintln(uint64_t n, mtype type, int index, int mlen);
static int getlen(uint64_t n);

/**
 * Perform long-hand multiplication on numbers `n1' and `n2', printing the
 * result to stdout. Errors written to stderr.
 */
void mul(const char* n1, const char* n2)
{
    int count = 0;
    uint64_t res = 0;
    uint64_t buf[BUF_LEN];

    uint64_t a = 0;
    uint64_t b = 0;

    sscanf(n1, "%llu", &a);
    sscanf(n2, "%llu", &b);

    for (uint64_t i = b, m = 1; i > 0; i /= RADIX) {
        int d = b % RADIX;

        uint64_t p = a * d;
        res += p * m;

        b /= RADIX;
        m *= RADIX;

        buf[count++] = p;
    }

    buf[count++] = SENTINEL;
    sscanf(n2, "%llu", &b);

    pprint(a, b, res, buf);
}

/**
 * Pretty-print the result of long-hand multiplication of `n1' and `n2'.
 */
void pprint(uint64_t n1, uint64_t n2,
            uint64_t res, uint64_t buf[BUF_LEN])
{
    int mlen = getlen(res);
    uint64_t dash = n1 > n2 ? n1 : n2;

    int minlen = getlen(n2) < getlen(n1) ?
                 getlen(n1) : getlen(n2);

    if (mlen < minlen + 2) {
        mlen = minlen + 2;
    }
    if (mlen == 2) {
        mlen = 3;
    }

    pprintln(n1, MULTIPLIER, SENTINEL, mlen);
    pprintln(n2, MULTIPLICAND, SENTINEL, mlen);
    pprintln(dash, DASH, SENTINEL, mlen);

    for (int i = 0; buf[i] != SENTINEL; ++i) {
        pprintln(buf[i], PARTIAL, i, mlen);
    }

    if (buf[1] != SENTINEL) {
        if (buf[0] != SENTINEL) {
            pprintln(dash, DASH, SENTINEL, mlen);
        }
        pprintln(res, PRODUCT, SENTINEL, mlen);
    }
}

/**
 * Pretty-print string `n' of type `mtype'. `index' is the zero-based index
 * of the line if it is a partial result and the total * length of the line
 * is `mlen'.
 */
void pprintln(uint64_t n, mtype type, int index, int mlen)
{
    int len = getlen(n);
    int lead = mlen - getlen(n * pow(RADIX, index));
    int trail = mlen - len - lead;

    switch (type) {
        case MULTIPLIER:
            printf("\t");
            for (int i = 0; i < lead-1; ++i) { printf(" "); }
            printf("%llu\n", n);
            break;

        case MULTIPLICAND:
            printf("\t");
            printf("x ");
            for (int i = 0; i < lead-3; ++i) { printf(" "); }
            printf("%llu\n", n);
            break;

        case PARTIAL:
            printf("\t");
            if (n == 0) {
                int i = 0;
                for ( ; i < mlen-index-1; ++i) { printf(" "); }
                printf("%llu", n);
                for ( ; i < mlen-1; ++i) { printf(" "); }
            }
            else {
                for (int i = 0; i < lead; ++i) { printf(" "); }
                printf("%llu", n);
                for (int i = 0; i < trail; ++i) { printf(" "); }
            }
            printf("\n");
            break;

        case PRODUCT:
            printf("\t");
            for (int i = 0; i < lead-1; ++i) { printf(" "); }
            printf("%llu\n", n);
            break;

        case DASH:
            printf("\t");
            for (int i = 0; i < mlen; ++i) { printf("-"); }
            printf("\n");
            break;
    }
}

/**
 * Get the length input `n' requires given radix `RADIX'.
 */
int getlen(uint64_t n)
{
    int len = 0;
    uint64_t temp = n;

    while (temp > 0) {
        len++;
        temp /= RADIX;
    }

    return len;
}

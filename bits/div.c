
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "div.h"


#define BUF_LEN   15
#define SENTINEL  -1
#define RADIX     10

typedef enum {
    QUOTIENT  = 0x01,
    DIVIDEND  = 0x02,
    DIVISOR   = 0x04,
    PARTIAL   = 0x08,
    SDASH     = 0x10,
    DASH      = 0x20
} mtype;


static void pprint(const char* n1, const char* n2, const char* res,
                   uint64_t p0[BUF_LEN], uint64_t p1[BUF_LEN], int plen);
static void pprintln(const char* n, mtype type, uint64_t m, int mlen);
static void getdigits(int buf[BUF_LEN], const char* str);
static int gettrail(uint64_t n, uint64_t nn);


/**
 * Perform long-hand division on numbers `n1' and `n2', printing the result
 * to stdout. Errors written to stderr.
 */
void div_(const char* n1, const char* n2)
{
    if (!strcmp(n2, "0")) {
        fprintf(stderr, "Error: division by 0.\n");
        exit(-1);
    }

    int a[BUF_LEN];
    memset(a, SENTINEL, BUF_LEN * sizeof(int));
    getdigits(a, n1);

    uint64_t b = 0;
    sscanf(n2, "%llu", &b);

    int rindex = 0;
    char res[BUF_LEN];
    memset(res, '\0', BUF_LEN * sizeof(char));

    int p0index = 0;
    uint64_t p0[BUF_LEN];
    memset(p0, SENTINEL, BUF_LEN * sizeof(uint64_t));

    int p1index = 0;
    uint64_t p1[BUF_LEN];
    memset(p1, SENTINEL, BUF_LEN * sizeof(uint64_t));

    int p = 0;

    for (int i = 0; a[i] != SENTINEL; ++i) {
        p = p * RADIX + a[i];

        if (strlen(res) > 0) {
            p1[p1index++] = p;
        }

        if (p >= b) {
            res[rindex++] = p / b + '0';
            p0[p0index++] = (p / b) * b;
            p = p % b;
        }
        else if (strlen(res) > 0) {
            p0[p0index++] = (p / b) * b;
            res[rindex++] = 0 + '0';
        }
    }

    if (res[0] == '\0') {
        strcpy(res, "0");
        p0[0] = 0;
        sscanf(n1, "%llu", &p1[0]);
        p0index = 1;
    }
    else {
        p1[p1index++] = p;
    }

    pprint(n1, n2, res, p0, p1, p0index);
}

/**
 * Pretty-print the result of long-hand division of `n1' and `n2' with
 * partial results `p0' and `p1' and result of length `plen'.
 */
void pprint(const char* n1, const char* n2, const char* res,
            uint64_t p0[BUF_LEN], uint64_t p1[BUF_LEN], int plen)
{
    int mlen = strlen(n1) + strlen(n2) + 3;  /* 3 for `` | '' */

    pprintln(res, QUOTIENT, SENTINEL, mlen);
    pprintln(NULL, SDASH, strlen(n2)+1, mlen);
    pprintln(n2, DIVISOR, SENTINEL, mlen);
    pprintln(n1, DIVIDEND, SENTINEL, mlen);

    int d = strlen(n1);
    int m = pow(RADIX, (strlen(res)));
    char n[BUF_LEN];
    memset(n, '\0', BUF_LEN * sizeof(char));

    for (int i = 0; i < plen; ++i) {
        if (i == 0) {
            m /= RADIX;
        }

        sprintf(n, "%llu", p0[i]);
        pprintln(n, PARTIAL, m, mlen);

        pprintln(NULL, DASH, d--, mlen);

        m /= RADIX;

        sprintf(n, "%llu", p1[i]);
        pprintln(n, PARTIAL, m, mlen);
    }
}

/**
 * Pretty-print string `n' of type `mtype'. n is of length `m' and the total
 * length of the line is `mlen'.
 */
void pprintln(const char* n, mtype type, uint64_t m, int mlen)
{
    int len = 0;
    int lead = 0;
    int trail = 0;
    uint64_t nn = 0;

    switch (type) {
        case QUOTIENT:
            len = strlen(n);
            lead = mlen - len;
            printf("\t");
            for (int i = 0; i < lead; ++i) { printf(" "); }
            printf("%s\n", n);
            break;

        case DIVIDEND:
            printf("%s\n", n);
            break;

        case DIVISOR:
            printf("\t%s | ", n);
            break;

        case PARTIAL:
            sscanf(n, "%llu", &nn);
            len = strlen(n);
            trail = gettrail(m, nn);
            lead = mlen - len - trail;
            printf("\t");
            for (int i = 0; i < lead; ++i) { printf(" "); }
            printf("%s", n);
            for (int i = 0; i < trail; ++i) { printf(" "); }
            printf("\n");
            break;

        case SDASH:
            lead = mlen - m;
            printf("\t");
            for (int i = 0; i < m; ++i) { printf(" "); }
            printf("/");
            for (int i = 0; i < lead-1; ++i) { printf("-"); }
            printf("\n");
            break;

        case DASH:
            lead = mlen - m;
            printf("\t");
            for (int i = 0; i < lead; ++i) { printf(" "); }
            for (int i = 0; i < m; ++i) { printf("-"); }
            printf("\n");

            break;
    }
}

/**
 * Get the integer values of the characters in string `str' and store them
 * in `buf'.
 */
void getdigits(int buf[BUF_LEN], const char* str)
{
    int i = 0;
    int len = strlen(str);

    for (i = 0; i < len; ++i) {
        buf[i] = str[i] - '0';
    }

    buf[i] = SENTINEL;
}

/**
 * Get the number of trailing spaces for `n' given `nn', the `expanded'
 * product of integer n.
 */
int gettrail(uint64_t n, uint64_t nn)
{
    int trail = 0;

    if (nn == 0) {
        nn = 1;  /* special case */
    }

    uint64_t temp = n * nn;

    while (temp > nn) {
        trail++;
        temp /= RADIX;
    }

    return trail;
}


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "modprod.h"
#include "util.h"


void modprod(const char* p1, const char* p2)
{
    if (strlen(p1) != 8 || strlen(p2) != 8) {
        fprintf(stderr, "Error: invalid modprod input, bailing.\n");
        exit(-1);
    }

    uint8_t a[4];
    uint8_t b[4];

    memset(a, 0, sizeof(a));
    memset(b, 0, sizeof(b));

    char buf[3];
    memset(buf, '\0', sizeof(buf));

    for (int i = 0, j = 0; i < 8; i += 2, ++j) {
        memcpy(buf, &p1[i], 2 * sizeof(char));
        int d = hex2dec(buf);
        a[j] = (uint8_t)d;

        memcpy(buf, &p2[i], 2 * sizeof(char));
        d = hex2dec(buf);
        b[j] = (uint8_t)d;
    }

    uint8_t d[4];
    memset(d, 0, sizeof(d));

    reverse(a);
    reverse(b);

    circlex(a, b, d);

    printf("{%02x}{%02x}{%02x}{%02x}", a[3], a[2], a[1], a[0]);
    printf(" CIRCLEX ");
    printf("{%02x}{%02x}{%02x}{%02x}", b[3], b[2], b[1], b[0]);
    printf(" = ");
    printf("{%02x}{%02x}{%02x}{%02x}\n", d[3], d[2], d[1], d[0]);
}

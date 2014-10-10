
#include <string.h>
#include <stdio.h>

#include "inverse.h"
#include "util.h"


static void extendedEuclideanAlgorithm(uint8_t a[5], uint8_t b[5]);
static void polydiv(uint8_t a[5], uint8_t b[5], uint8_t quo[5], uint8_t rem[5], int finalStep);

void inverse(const char* poly)
{
    uint8_t ax[5];
    uint8_t bx[5];

    memset(ax, 0, 5 * sizeof(uint8_t));
    memset(bx, 0, 5 * sizeof(uint8_t));

    char str[3];
    memset(str, '\0', 3 * sizeof(char));

    for (int i = 0; i < 4; ++i) {
        memcpy(str, &poly[2*i], 2);
        ax[i+1] = hex2dec(str);
    }

    bx[0] = 0x1;
    bx[1] = 0x0;
    bx[2] = 0x0;
    bx[3] = 0x0;
    bx[4] = 0x1;

    extendedEuclideanAlgorithm(ax, bx);
}

void extendedEuclideanAlgorithm(uint8_t a[5], uint8_t b[5])
{
    uint8_t quo[5];
    uint8_t rem[5];
    uint8_t rem1[5];
    uint8_t rem2[5];

    uint8_t x[4];
    uint8_t x1[4];
    uint8_t x2[4];
    uint8_t temp[4];

    memset(quo, 0, 5 * sizeof(uint8_t));
    memset(rem, 0, 5 * sizeof(uint8_t));
    memset(rem1, 0, 5 * sizeof(uint8_t));
    memset(rem2, 0, 5 * sizeof(uint8_t));

    memset(x, 0, 4 * sizeof(uint8_t));
    memset(x1, 0, 4 * sizeof(uint8_t));
    memset(x2, 0, 4 * sizeof(uint8_t));
    memset(temp, 0, 4 * sizeof(uint8_t));

    memcpy(rem1, b, 5 * sizeof(uint8_t));
    memcpy(rem2, a, 5 * sizeof(uint8_t));

    x1[0] = 0x0;
    x1[1] = 0x0;
    x1[2] = 0x0;
    x1[3] = 0x0;

    x2[0] = 0x0;
    x2[1] = 0x0;
    x2[2] = 0x0;
    x2[3] = 0x1;

    printf("i=1, rem[i]={%02x}{%02x}{%02x}{%02x}, ", 0, 0, 0, 1);
    printf("quo[i]={%02x}{%02x}{%02x}{%02x}, ", 0, 0, 0, 0);
    printf("aux[i]={%02x}{%02x}{%02x}{%02x}\n", 0, 0, 0, 0);

    printf("i=2, rem[i]={%02x}{%02x}{%02x}{%02x}, ", rem2[1], rem2[2], rem2[3], rem2[4]);
    printf("quo[i]={%02x}{%02x}{%02x}{%02x}, ", 0, 0, 0, 0);
    printf("aux[i]={%02x}{%02x}{%02x}{%02x}\n", 0, 0, 0, 1);

    polydiv(rem1, rem2, quo, rem, 0);

    reverse(&quo[1]);
    reverse(x2);
    circlex(&quo[1], x2, temp);
    reverse(temp);

    x[0] = temp[0] ^ x1[0];
    x[1] = temp[1] ^ x1[1];
    x[2] = temp[2] ^ x1[2];
    x[3] = temp[3] ^ x1[3];

    reverse(&quo[1]);
    reverse(x2);

    memcpy(rem1, rem2, 5 * sizeof(uint8_t));
    memcpy(rem2, rem, 5 * sizeof(uint8_t));

    memcpy(x1, x2, 4 * sizeof(uint8_t));
    memcpy(x2, x, 4 * sizeof(uint8_t));

    printf("i=3, rem[i]={%02x}{%02x}{%02x}{%02x}, ", rem[1], rem[2], rem[3], rem[4]);
    printf("quo[i]={%02x}{%02x}{%02x}{%02x}, ", quo[1], quo[2], quo[3], quo[4]);
    printf("aux[i]={%02x}{%02x}{%02x}{%02x}\n", x[0], x[1], x[2], x[3]);

    polydiv(rem1, rem2, quo, rem, 0);

    reverse(&quo[1]);
    reverse(x2);
    circlex(&quo[1], x2, temp);
    reverse(temp);

    x[0] = temp[0] ^ x1[0];
    x[1] = temp[1] ^ x1[1];
    x[2] = temp[2] ^ x1[2];
    x[3] = temp[3] ^ x1[3];

    reverse(&quo[1]);
    reverse(x2);

    memcpy(rem1, rem2, 5 * sizeof(uint8_t));
    memcpy(rem2, rem, 5 * sizeof(uint8_t));

    memcpy(x1, x2, 4 * sizeof(uint8_t));
    memcpy(x2, x, 4 * sizeof(uint8_t));

    if (rem1[4] == 0x0) {
        printf("{%02x}{%02x}{%02x}{%02x} does not have a multiplicative inverse.\n", a[1], a[2], a[3], a[4]);
        return;
    }

    printf("i=4, rem[i]={%02x}{%02x}{%02x}{%02x}, ", rem[1], rem[2], rem[3], rem[4]);
    printf("quo[i]={%02x}{%02x}{%02x}{%02x}, ", quo[1], quo[2], quo[3], quo[4]);
    printf("aux[i]={%02x}{%02x}{%02x}{%02x}\n", x[0], x[1], x[2], x[3]);

    polydiv(rem1, rem2, quo, rem, 0);

    reverse(&quo[1]);
    reverse(x2);
    circlex(&quo[1], x2, temp);
    reverse(temp);

    x[0] = temp[0] ^ x1[0];
    x[1] = temp[1] ^ x1[1];
    x[2] = temp[2] ^ x1[2];
    x[3] = temp[3] ^ x1[3];

    reverse(&quo[1]);
    reverse(x2);

    memcpy(rem1, rem2, 5 * sizeof(uint8_t));
    memcpy(rem2, rem, 5 * sizeof(uint8_t));

    memcpy(x1, x2, 4 * sizeof(uint8_t));
    memcpy(x2, x, 4 * sizeof(uint8_t));

    if (rem1[4] == 0x0) {
        printf("{%02x}{%02x}{%02x}{%02x} does not have a multiplicative inverse.\n", a[1], a[2], a[3], a[4]);
        return;
    }

    printf("i=5, rem[i]={%02x}{%02x}{%02x}{%02x}, ", rem[1], rem[2], rem[3], rem[4]);
    printf("quo[i]={%02x}{%02x}{%02x}{%02x}, ", quo[1], quo[2], quo[3], quo[4]);
    printf("aux[i]={%02x}{%02x}{%02x}{%02x}\n", x[0], x[1], x[2], x[3]);

    polydiv(rem1, rem2, quo, rem, 1);

    reverse(&quo[1]);
    reverse(x2);
    circlex(&quo[1], x2, temp);
    reverse(temp);

    x[0] = temp[0] ^ x1[0];
    x[1] = temp[1] ^ x1[1];
    x[2] = temp[2] ^ x1[2];
    x[3] = temp[3] ^ x1[3];

    reverse(&quo[1]);
    reverse(x2);

    memcpy(rem1, rem2, 5 * sizeof(uint8_t));
    memcpy(rem2, rem, 5 * sizeof(uint8_t));

    memcpy(x1, x2, 4 * sizeof(uint8_t));
    memcpy(x2, x, 4 * sizeof(uint8_t));

    if (rem1[4] == 0x0) {
        printf("{%02x}{%02x}{%02x}{%02x} does not have a multiplicative inverse.\n", a[1], a[2], a[3], a[4]);
        return;
    }
    else {
        printf("i=6, rem[i]={%02x}{%02x}{%02x}{%02x}, ", rem[1], rem[2], rem[3], rem[4]);
        printf("quo[i]={%02x}{%02x}{%02x}{%02x}, ", quo[1], quo[2], quo[3], quo[4]);
        printf("aux[i]={%02x}{%02x}{%02x}{%02x}\n", x[0], x[1], x[2], x[3]);
        printf("Multiplicative inverse of {%02x}{%02x}{%02x}{%02x} is {%02x}{%02x}{%02x}{%02x}\n", a[1], a[2], a[3], a[4], x[0], x[1], x[2], x[3]);
    }
}

void polydiv(uint8_t a[5], uint8_t b[5], uint8_t quo[5], uint8_t rem[5], int finalStep)
{
    uint8_t temp[4];
    uint8_t res[4];

    memset(temp, 0, 4 * sizeof(uint8_t));
    memset(res, 0, 4 * sizeof(uint8_t));

    memcpy(rem, a, 5 * sizeof(uint8_t));

    int i;
    for (i = 0; i < 5; ++i) {
        if (rem[i] != 0) {
            break;
        }
    }

    int j;
    for (j = 0; j < 5; ++j) {
        if (b[j] != 0)
            break;
    }

    uint8_t inv = gfinverse(b[j]);
    temp[3] = gfdot(inv, rem[i]);

    reverse(temp);
    circlex(&b[1], temp, res);
    reverse(temp);


    quo[3] = temp[3];

    int k;
    for (k = 0; k < 4; ++k) {
        if (res[k] != 0) {
            break;
        }
    }

    if (k   < 4 && i   < 5) rem[i]   ^= res[k];
    if (k+1 < 4 && i+1 < 5) rem[i+1] ^= res[k+1];
    if (k+2 < 4 && i+2 < 5) rem[i+2] ^= res[k+2];
    if (k+3 < 4 && i+3 < 5) rem[i+3] ^= res[k+3];

    i += 1;

    memset(temp, 0, 4 * sizeof(uint8_t));
    memset(res, 0, 4 * sizeof(uint8_t));

    temp[3] = gfdot(inv, rem[i]);

    reverse(temp);
    circlex(&b[1], temp, res);
    reverse(temp);

    quo[4] = temp[3];

    for (k = 0; k < 4; ++k) {
        if (res[k] != 0) {
            break;
        }
    }

    if (finalStep) {
        rem[0] = 0x0;
        rem[1] = 0x0;
        rem[2] = 0x0;
        rem[3] = 0x0;
        rem[4] = 0x1;

        res[k] ^= 0x1;
        quo[4] = gfdot(res[k], inv);
    }
    else {
        if (k   < 4 && i   < 5) rem[i]   ^= res[k];
        if (k+1 < 4 && i+1 < 5) rem[i+1] ^= res[k+1];
        if (k+2 < 4 && i+2 < 5) rem[i+2] ^= res[k+2];
        if (k+3 < 4 && i+3 < 5) rem[i+3] ^= res[k+3];
    }
}


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "util.h"
#include "defines.h"


static void swap(uint8_t* a, uint8_t* b);

/*
 * Multiplicative inverses of {00} through {ff} in GF(2^8).
 */
uint8_t InverseTable[] = {
         /* 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
 /* 0 */ 0x00, 0x01, 0x8d, 0xf6, 0xcb, 0x52, 0x7b, 0xd1, 0xe8, 0x4f, 0x29, 0xc0, 0xb0, 0xe1, 0xe5, 0xc7,
 /* 1 */ 0x74, 0xb4, 0xaa, 0x4b, 0x99, 0x2b, 0x60, 0x5f, 0x58, 0x3f, 0xfd, 0xcc, 0xff, 0x40, 0xee, 0xb2,
 /* 2 */ 0x3a, 0x6e, 0x5a, 0xf1, 0x55, 0x4d, 0xa8, 0xc9, 0xc1, 0x0a, 0x98, 0x15, 0x30, 0x44, 0xa2, 0xc2,
 /* 3 */ 0x2c, 0x45, 0x92, 0x6c, 0xf3, 0x39, 0x66, 0x42, 0xf2, 0x35, 0x20, 0x6f, 0x77, 0xbb, 0x59, 0x19,
 /* 4 */ 0x1d, 0xfe, 0x37, 0x67, 0x2d, 0x31, 0xf5, 0x69, 0xa7, 0x64, 0xab, 0x13, 0x54, 0x25, 0xe9, 0x09,
 /* 5 */ 0xed, 0x5c, 0x05, 0xca, 0x4c, 0x24, 0x87, 0xbf, 0x18, 0x3e, 0x22, 0xf0, 0x51, 0xec, 0x61, 0x17,
 /* 6 */ 0x16, 0x5e, 0xaf, 0xd3, 0x49, 0xa6, 0x36, 0x43, 0xf4, 0x47, 0x91, 0xdf, 0x33, 0x93, 0x21, 0x3b,
 /* 7 */ 0x79, 0xb7, 0x97, 0x85, 0x10, 0xb5, 0xba, 0x3c, 0xb6, 0x70, 0xd0, 0x06, 0xa1, 0xfa, 0x81, 0x82,
 /* 8 */ 0x83, 0x7e, 0x7f, 0x80, 0x96, 0x73, 0xbe, 0x56, 0x9b, 0x9e, 0x95, 0xd9, 0xf7, 0x02, 0xb9, 0xa4,
 /* 9 */ 0xde, 0x6a, 0x32, 0x6d, 0xd8, 0x8a, 0x84, 0x72, 0x2a, 0x14, 0x9f, 0x88, 0xf9, 0xdc, 0x89, 0x9a,
 /* a */ 0xfb, 0x7c, 0x2e, 0xc3, 0x8f, 0xb8, 0x65, 0x48, 0x26, 0xc8, 0x12, 0x4a, 0xce, 0xe7, 0xd2, 0x62,
 /* b */ 0x0c, 0xe0, 0x1f, 0xef, 0x11, 0x75, 0x78, 0x71, 0xa5, 0x8e, 0x76, 0x3d, 0xbd, 0xbc, 0x86, 0x57,
 /* c */ 0x0b, 0x28, 0x2f, 0xa3, 0xda, 0xd4, 0xe4, 0x0f, 0xa9, 0x27, 0x53, 0x04, 0x1b, 0xfc, 0xac, 0xe6,
 /* d */ 0x7a, 0x07, 0xae, 0x63, 0xc5, 0xdb, 0xe2, 0xea, 0x94, 0x8b, 0xc4, 0xd5, 0x9d, 0xf8, 0x90, 0x6b,
 /* e */ 0xb1, 0x0d, 0xd6, 0xeb, 0xc6, 0x0e, 0xcf, 0xad, 0x08, 0x4e, 0xd7, 0xe3, 0x5d, 0x50, 0x1e, 0xb3,
 /* f */ 0x5b, 0x23, 0x38, 0x34, 0x68, 0x46, 0x03, 0x8c, 0xdd, 0x9c, 0x7d, 0xa0, 0xcd, 0x1a, 0x41, 0x1c
};

void AddRoundKey(uint8_t state[4*Nb], uint32_t* w)
{
    for (int i = 0; i < Nb; ++i) {
        uint8_t a0 = *(w+i) >> 24;
        uint8_t a1 = (*(w+i) <<  8) >> 24;
        uint8_t a2 = (*(w+i) << 16) >> 24;
        uint8_t a3 = (*(w+i) << 24) >> 24;

        state[Nb*i+0] ^= a0;
        state[Nb*i+1] ^= a1;
        state[Nb*i+2] ^= a2;
        state[Nb*i+3] ^= a3;
    }
}

int hex2dec(const char* hexstring)
{
    int d = 0;
    const char* c = hexstring;

    if (strlen(hexstring) != 2 ||
        ( (c[0] < '0' || c[0] > '9') &&
          (c[0] < 'a' || c[0] > 'f') &&
          (c[0] < 'A' || c[0] > 'F'))
        ||
        ( (c[1] < '0' || c[1] > '9') &&
          (c[1] < 'a' || c[1] > 'f') &&
          (c[1] < 'A' || c[1] > 'F')))
    {
        fprintf(stderr, "Error: invalid hexstring, `%s'.\n", hexstring);
        exit(-1);
    }
    else {
        sscanf(hexstring, "%x", &d);
    }

    return d;
}

uint8_t xtime(uint8_t b)
{
    int flag = b & 0x80 ? 1 : 0;

    b <<= 1;

    if (flag) {
        b ^= 0x1b;
    }

    return b;
}

uint8_t gfdot(uint8_t a, uint8_t b)
{
    uint8_t res = 0x0;
    uint8_t v[8];

    v[0] = a;
    v[1] = xtime(v[0]);
    v[2] = xtime(v[1]);
    v[3] = xtime(v[2]);
    v[4] = xtime(v[3]);
    v[5] = xtime(v[4]);
    v[6] = xtime(v[5]);
    v[7] = xtime(v[6]);

    int flag = 0;

    if (b & 0x80) {
        res = flag ? res ^ v[7] : v[7];
        flag |= 1;
    }
    if (b & 0x40) {
        res = flag ? res ^ v[6] : v[6];
        flag |= 1;
    }
    if (b & 0x20) {
        res = flag ? res ^ v[5] : v[5];
        flag |= 1;
    }
    if (b & 0x10) {
        res = flag ? res ^ v[4] : v[4];
        flag |= 1;
    }
    if (b & 0x08) {
        res = flag ? res ^ v[3] : v[3];
        flag |= 1;
    }
    if (b & 0x04) {
        res = flag ? res ^ v[2] : v[2];
        flag |= 1;
    }
    if (b & 0x02) {
        res = flag ? res ^ v[1] : v[1];
        flag |= 1;
    }
    if (b & 0x01) {
        res = flag ? res ^ v[0] : v[0];
        flag |= 1;
    }

    return res;
}

/*
 * a = [a0, a1, a2, a3]
 * b = [b0, b1, b2, b3]
 * d = [d0, d1, d2, d3]
 *
 * ax = a3 x^3 + a2 x^2 + a1 x + a0
 * bx = b3 x^3 + b2 x^2 + b1 x + b0
 * dx = d3 x^3 + d2 x^2 + d1 x + d0
 *
 * [d0]   [a0 a3 a2 a1] [b0]
 * [d1] = [a1 a0 a3 a2] [b1]
 * [d2]   [a2 a1 a0 a3] [b2]
 * [d3]   [a3 a2 a1 a0] [b3]
 */
void circlex(uint8_t a[4], uint8_t b[4], uint8_t d[4])
{
    d[0] = gfdot(a[0], b[0]) ^ gfdot(a[3], b[1]) ^
           gfdot(a[2], b[2]) ^ gfdot(a[1], b[3]);

    d[1] = gfdot(a[1], b[0]) ^ gfdot(a[0], b[1]) ^
           gfdot(a[3], b[2]) ^ gfdot(a[2], b[3]);

    d[2] = gfdot(a[2], b[0]) ^ gfdot(a[1], b[1]) ^
           gfdot(a[0], b[2]) ^ gfdot(a[3], b[3]);

    d[3] = gfdot(a[3], b[0]) ^ gfdot(a[2], b[1]) ^
           gfdot(a[1], b[2]) ^ gfdot(a[0], b[3]);
}

uint8_t gfinverse(uint8_t hexstring)
{
    return InverseTable[hexstring];
}

void printRcon()
{
    int i = 0;

    uint8_t Rcon[16];
    memset(Rcon, 0, 16 * sizeof(uint8_t));

    Rcon[1] = 0x01;
    Rcon[2] = 0x02;

    for (i = 3; i < 16; ++i) {
        Rcon[i] = gfdot(Rcon[i-1], 0x02);
    }

    for (i = 0; i < 16; ++i) {
        printf("Rcon[%d]: 0x%02x\n", i, Rcon[i]);
    }
}

void reverse(uint8_t buf[4])
{
    swap(&buf[0], &buf[3]);
    swap(&buf[1], &buf[2]);
}

void swap(uint8_t* a, uint8_t* b)
{
    uint8_t temp = *a;

    *a = *b;
    *b = temp;
}

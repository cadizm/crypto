
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "util.h"


static uint8_t dec2hex(uint8_t d);
static void swap(int buf[16][48], int i, int j);

/*
 * Return hexstring representation of buf in res.
 *
 * buf: an array of size `n' of binary digits.
 * res: should be a pre-allocated char buffer
 *      of the appropriate size.
 */
void hexstring(int* buf, int n, uint8_t* res)
{
    int k = 0;

    for (int i = 0; i < n; i += 4) {
        uint8_t c = 0x0;

        c |= buf[i+0] == 1 ? 0x08 : 0x0;
        c |= buf[i+1] == 1 ? 0x04 : 0x0;
        c |= buf[i+2] == 1 ? 0x02 : 0x0;
        c |= buf[i+3] == 1 ? 0x01 : 0x0;

        res[k++] = dec2hex(c);
    }
}

uint8_t dec2hex(uint8_t d)
{
    switch (d) {
        case  0: return '0';
        case  1: return '1';
        case  2: return '2';
        case  3: return '3';
        case  4: return '4';
        case  5: return '5';
        case  6: return '6';
        case  7: return '7';
        case  8: return '8';
        case  9: return '9';
        case 10: return 'a';
        case 11: return 'b';
        case 12: return 'c';
        case 13: return 'd';
        case 14: return 'e';
        case 15: return 'f';
        default: return 'X';
    }
}

void unpack(uint8_t buf[8], int res[64])
{
    for (int i = 0; i < 8; ++i) {
        res[8*i+0] = 0x80 & buf[i] ? 1 : 0;
        res[8*i+1] = 0x40 & buf[i] ? 1 : 0;
        res[8*i+2] = 0x20 & buf[i] ? 1 : 0;
        res[8*i+3] = 0x10 & buf[i] ? 1 : 0;
        res[8*i+4] = 0x08 & buf[i] ? 1 : 0;
        res[8*i+5] = 0x04 & buf[i] ? 1 : 0;
        res[8*i+6] = 0x02 & buf[i] ? 1 : 0;
        res[8*i+7] = 0x01 & buf[i] ? 1 : 0;
    }
}

void pack(int buf[64], uint8_t res[8])
{
    for (int i = 0; i < 8; ++i) {
        uint8_t c = 0x0;

        c |= buf[8*i+0] == 1 ? 0x80 : 0x0;
        c |= buf[8*i+1] == 1 ? 0x40 : 0x0;
        c |= buf[8*i+2] == 1 ? 0x20 : 0x0;
        c |= buf[8*i+3] == 1 ? 0x10 : 0x0;
        c |= buf[8*i+4] == 1 ? 0x08 : 0x0;
        c |= buf[8*i+5] == 1 ? 0x04 : 0x0;
        c |= buf[8*i+6] == 1 ? 0x02 : 0x0;
        c |= buf[8*i+7] == 1 ? 0x01 : 0x0;

        res[i] = c;
    }
}

void reverse(int buf[16][48], int n)
{
    if (n < 2) {
        return;
    }

    else if (n == 2) {
        swap(buf, 0, 1);
    }

    else if (n % 2 == 0) {
        int i = 0;
        int j = n - 1;

        while (i < j) {
            swap(buf, i, j);
            i += 1;
            j -= 1;
        }
    }

    else {
        int i = 0;
        int j = n - 1;

        while (i != j) {
            swap(buf, i, j);
            i += 1;
            j -= 1;
        }
    }
}

void swap(int buf[16][48], int i, int j)
{
    int temp[48];

    memcpy(temp, &buf[i], 48 * sizeof(int));
    memcpy(&buf[i], &buf[j], 48 * sizeof(int));
    memcpy(&buf[j], temp, 48 * sizeof(int));
}

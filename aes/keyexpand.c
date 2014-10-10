
#include <string.h>
#include <stdio.h>

#include "keyexpand.h"
#include "tablecheck.h"
#include "util.h"


static uint32_t Rcon[] = {
    0x00000000, 0x01000000, 0x02000000, 0x04000000,
    0x08000000, 0x10000000, 0x20000000, 0x40000000,
    0x80000000, 0x1b000000, 0x36000000, 0x6c000000,
    0xd8000000, 0xab000000, 0x4d000000, 0x9a000000
};

static uint32_t SubWord(uint32_t w);
static uint32_t RotWord(uint32_t w);
static uint32_t word(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3);

static int* Table[3];

void keyexpand(const char* k, const char* t, uint32_t w[Nb*(Nr+1)], int bPrint)
{
    uint8_t key[4*Nk];
    memset(key, 0, 4*Nk * sizeof(uint8_t));

    char str[3];
    memset(str, '\0', 3 * sizeof(char));

    for (int i = 0; i < 4*Nk; ++i) {
        memcpy(str, &k[2*i], 2);
        key[i] = hex2dec(str);
    }

    int S[256];
    int P[4];
    int INVP[4];

    memset(&S[0], 0, sizeof(S));
    memset(&P[0], 0, sizeof(P));
    memset(&INVP[0], 0, sizeof(INVP));

    Table[0] = &S[0];
    Table[1] = &P[0];
    Table[2] = &INVP[0];

    tablecheck(t, Table);

    KeyExpansion(key, w);

    if (bPrint) {
        for (int i = 0; i < Nb*(Nr+1); ++i) {
            printf("w[%2d]: %08x\n", i, w[i]);
        }
    }
}

void KeyExpansion(uint8_t key[4*Nk], uint32_t w[Nb*(Nr+1)])
{
    uint32_t temp = 0;

    int i = 0;

    while (i < Nk) {
        w[i] = word(key[4*i], key[4*i+1], key[4*i+2], key[4*i+3]);
        i += 1;
    }

    i = Nk;

    while (i < Nb * (Nr+1)) {
        temp = w[i-1];
        if (i % Nk == 0) {
            temp = SubWord(RotWord(temp)) ^ Rcon[i/Nk];
        }
        else if (Nk > 6 && i % Nk == 4) {
            temp = SubWord(temp);
        }
        w[i] = w[i-Nk] ^ temp;
        i += 1;
    }
}

uint32_t SubWord(uint32_t w)
{
    int* S = Table[0];
    uint32_t res = 0;

    uint8_t a0 = w >> 24;
    uint8_t a1 = (w <<  8) >> 24;
    uint8_t a2 = (w << 16) >> 24;
    uint8_t a3 = (w << 24) >> 24;

    int x = a0 >> 4;
    int y = a0 & 0x0f;
    res = S[0x10*x+y] << 24;

    x = a1 >> 4;
    y = a1 & 0x0f;
    res |= S[0x10*x+y] << 16;

    x = a2 >> 4;
    y = a2 & 0x0f;
    res |= S[0x10*x+y] << 8;

    x = a3 >> 4;
    y = a3 & 0x0f;
    res |= S[0x10*x+y];

    return res;
}

uint32_t RotWord(uint32_t w)
{
    uint32_t res = w >> 24;
    res |= w << 8;

    return res;
}

uint32_t word(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3)
{
    uint32_t res = 0x0;

    res |= a0;
    res = (res << 8) | a1;
    res = (res << 8) | a2;
    res = (res << 8) | a3;

    return res;
}

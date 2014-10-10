
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "decrypt.h"
#include "keyexpand.h"
#include "tablecheck.h"
#include "util.h"


static void InvCipher(uint8_t in[4*Nb], uint8_t out[4*Nb], uint32_t w[Nb*(Nr+1)]);
static void InvSubBytes(uint8_t state[4*Nb]);
static void InvShiftRows(uint8_t state[4*Nb]);
static void InvMixColumns(uint8_t state[4*Nb]);

static void buildInvS(int S[256], int INVS[256]);

static int* Table[4];
static int m_round = 0;

void decrypt(const char* k, const char* t, FILE* fp)
{
    int S[256];
    int P[4];
    int INVS[256];
    int INVP[4];

    memset(&S[0], 0, sizeof(S));
    memset(&P[0], 0, sizeof(P));
    memset(&INVS[0], 0, sizeof(INVS));
    memset(&INVP[0], 0, sizeof(INVP));

    Table[0] = &S[0];
    Table[1] = &P[0];
    Table[2] = &INVP[0];

    tablecheck(t, Table);

    buildInvS(S, INVS);

    Table[3] = &INVS[0];

    uint32_t w[Nb*(Nr+1)];
    memset(w, 0, Nb*(Nr+1) * sizeof(uint32_t));

    keyexpand(k, t, w, 0);

    uint8_t in[4*Nb];
    uint8_t out[4*Nb];

    memset(in, 0, 4*Nb * sizeof(uint8_t));
    memset(out, 0, 4*Nb * sizeof(uint8_t));

    int nObj = 0;
    for (int i =0; i < 4*Nb; ++i) {
        nObj = fread(&in[i], 1, 1, fp);
        if (nObj == 0) {
            if (i == 0) {
                return;
            }
            else {
                fprintf(stderr, "Error: input < 16 bytes, bailing.\n");
                exit(-1);
            }
        }
    }

    InvCipher(in, out, w);
#ifndef DEBUG_
    fwrite(out, 1, 4*Nb, stdout);
#endif /* !DEBUG_ */

    while (!feof(fp)) {
        memset(in, 0, 4*Nb * sizeof(uint8_t));
        memset(out, 0, 4*Nb * sizeof(uint8_t));
        nObj = 0;
        for (int i =0; i < 4*Nb; ++i) {
            nObj = fread(&in[i], 1, 1, fp);
            if (nObj == 0) {
                if (i == 0) {
                    return;
                }
                else {
                    break;
                }
            }
        }

        InvCipher(in, out, w);
#ifndef DEBUG_
        fwrite(out, 1, 4*Nb, stdout);
#endif /* !DEBUG_ */
    }
}

void InvCipher(uint8_t in[4*Nb], uint8_t out[4*Nb], uint32_t w[Nb*(Nr+1)])
{
#ifdef DEBUG_
    printf("round[%2d].iinput%3s", 0, " ");
    for (int i = 0; i < 4*Nb; ++i) {
        printf("%02x", in[i]);
    }
    printf("\n");
#endif /* DEBUG_ */

#ifdef DEBUG_
    printf("round[%2d].ik_sch%3s", 0, " ");
    for (int i = (Nr-m_round)*Nb; i < (Nr-m_round)*Nb+Nk; ++i) {
        printf("%08x", w[i]);
    }
    printf("\n");
#endif /* DEBUG_ */

    uint8_t state[4*Nb];

    memcpy(state, in, 4*Nb * sizeof(uint8_t));

    AddRoundKey(state, &w[(Nr-m_round)*Nb]);

    for (m_round = 1; m_round <= Nr-1; ++m_round) {
#ifdef DEBUG_
        printf("round[%2d].istart%3s", m_round, " ");
        for (int i = 0; i < Nb; ++i) {
            printf("%02x%02x%02x%02x", state[Nb*i+0], state[Nb*i+1], state[Nb*i+2], state[Nb*i+3]);
        }
        printf("\n");
#endif /* DEBUG_ */

        InvShiftRows(state);

#ifdef DEBUG_
        printf("round[%2d].is_row%3s", m_round, " ");
        for (int i = 0; i < Nb; ++i) {
            printf("%02x%02x%02x%02x", state[Nb*i+0], state[Nb*i+1], state[Nb*i+2], state[Nb*i+3]);
        }
        printf("\n");
#endif /* DEBUG_ */

        InvSubBytes(state);

#ifdef DEBUG_
    printf("round[%2d].is_box%3s", m_round, " ");
    for (int i = 0; i < Nb; ++i) {
        printf("%02x%02x%02x%02x", state[Nb*i+0], state[Nb*i+1], state[Nb*i+2], state[Nb*i+3]);
    }
    printf("\n");
#endif /* DEBUG_ */

#ifdef DEBUG_
    printf("round[%2d].ik_sch%3s", m_round, " ");
    for (int i = (Nr-m_round)*Nb; i < (Nr-m_round)*Nb+Nk; ++i) {
        printf("%08x", w[i]);
    }
    printf("\n");
#endif /* DEBUG_ */

        AddRoundKey(state, &w[(Nr-m_round)*Nb]);

#ifdef DEBUG_
        printf("round[%2d].ik_add%3s", m_round, " ");
        for (int i = 0; i < Nb; ++i) {
            printf("%02x%02x%02x%02x", state[Nb*i+0], state[Nb*i+1], state[Nb*i+2], state[Nb*i+3]);
        }
        printf("\n");
#endif /* DEBUG_ */

        InvMixColumns(state);
    }

#ifdef DEBUG_
        printf("round[%2d].istart%3s", m_round, " ");
        for (int i = 0; i < Nb; ++i) {
            printf("%02x%02x%02x%02x", state[Nb*i+0], state[Nb*i+1], state[Nb*i+2], state[Nb*i+3]);
        }
        printf("\n");
#endif /* DEBUG_ */

    InvShiftRows(state);

#ifdef DEBUG_
        printf("round[%2d].is_row%3s", m_round, " ");
        for (int i = 0; i < Nb; ++i) {
            printf("%02x%02x%02x%02x", state[Nb*i+0], state[Nb*i+1], state[Nb*i+2], state[Nb*i+3]);
        }
        printf("\n");
#endif /* DEBUG_ */

    InvSubBytes(state);

#ifdef DEBUG_
    printf("round[%2d].is_box%3s", m_round, " ");
    for (int i = 0; i < Nb; ++i) {
        printf("%02x%02x%02x%02x", state[Nb*i+0], state[Nb*i+1], state[Nb*i+2], state[Nb*i+3]);
    }
    printf("\n");
#endif /* DEBUG_ */

#ifdef DEBUG_
    printf("round[%2d].ik_sch%3s", m_round, " ");
    for (int i = (Nr-m_round)*Nb; i < (Nr-m_round)*Nb+Nk; ++i) {
        printf("%08x", w[i]);
    }
    printf("\n");
#endif /* DEBUG_ */

    AddRoundKey(state, &w[(Nr-m_round)*Nb]);

    memcpy(out, state, 4*Nb * sizeof(uint8_t));

#ifdef DEBUG_
    printf("round[%2d].ioutput%2s", m_round, " ");
    for (int i = 0; i < Nb; ++i) {
        printf("%02x%02x%02x%02x", out[Nb*i+0], out[Nb*i+1], out[Nb*i+2], out[Nb*i+3]);
    }
    printf("\n");
#endif /* DEBUG_ */
}

void InvSubBytes(uint8_t state[4*Nb])
{
    int* INVS = Table[3];

    for (int i = 0; i < Nb; ++i) {
        uint8_t a0 = state[Nb*i+0];
        uint8_t a1 = state[Nb*i+1];
        uint8_t a2 = state[Nb*i+2];
        uint8_t a3 = state[Nb*i+3];

        int x = a0 >> 4;
        int y = a0 & 0x0f;
        state[Nb*i+0] = INVS[0x10*x+y];

        x = a1 >> 4;
        y = a1 & 0x0f;
        state[Nb*i+1] = INVS[0x10*x+y];

        x = a2 >> 4;
        y = a2 & 0x0f;
        state[Nb*i+2] = INVS[0x10*x+y];

        x = a3 >> 4;
        y = a3 & 0x0f;
        state[Nb*i+3] = INVS[0x10*x+y];
    }
}

void InvShiftRows(uint8_t state[4*Nb])
{
    uint8_t temp[4*Nb];
    memset(temp, 0, 4*Nb * sizeof(uint8_t));

    for (int i = 0; i < Nb; ++i) {
        temp[i+(Nb*0)] = state[Nb*i+0];
        temp[i+(Nb*1)] = state[Nb*i+1];
        temp[i+(Nb*2)] = state[Nb*i+2];
        temp[i+(Nb*3)] = state[Nb*i+3];
    }

    uint8_t buf[4];
    memset(buf, 0, 4 * sizeof(uint8_t));

    buf[0] = temp[Nb*1+3];
    temp[Nb*1+3] = temp[Nb*1+2];
    temp[Nb*1+2] = temp[Nb*1+1];
    temp[Nb*1+1] = temp[Nb*1+0];
    temp[Nb*1+0] = buf[0];

    memset(buf, 0, 4 * sizeof(uint8_t));

    buf[0] = temp[Nb*2+3];
    buf[1] = temp[Nb*2+2];
    temp[Nb*2+3] = temp[Nb*2+1];
    temp[Nb*2+2] = temp[Nb*2+0];
    temp[Nb*2+1] = buf[0];
    temp[Nb*2+0] = buf[1];

    memset(buf, 0, 4 * sizeof(uint8_t));

    buf[0] = temp[Nb*3+1];
    buf[1] = temp[Nb*3+2];
    buf[2] = temp[Nb*3+3];
    temp[Nb*3+3] = temp[Nb*3+0];
    temp[Nb*3+2] = buf[2];
    temp[Nb*3+1] = buf[1];
    temp[Nb*3+0] = buf[0];

    for (int i = 0; i < Nb; ++i) {
        state[Nb*i+0] = temp[i+(Nb*0)];
        state[Nb*i+1] = temp[i+(Nb*1)];
        state[Nb*i+2] = temp[i+(Nb*2)];
        state[Nb*i+3] = temp[i+(Nb*3)];
    }
}

void InvMixColumns(uint8_t state[4*Nb])
{
    int* P = Table[2];

    uint8_t a[4];
    uint8_t b[4];
    uint8_t d[4];

    memset(a, 0, 4 * sizeof(uint8_t));
    memset(d, 0, 4 * sizeof(uint8_t));

    b[3] = P[0];
    b[2] = P[1];
    b[1] = P[2];
    b[0] = P[3];

    for (int i = 0; i < Nb; ++i) {
        memset(a, 0, 4 * sizeof(uint8_t));

        a[0] = state[Nb*i+0];
        a[1] = state[Nb*i+1];
        a[2] = state[Nb*i+2];
        a[3] = state[Nb*i+3];

        memset(d, 0, 4 * sizeof(uint8_t));

        circlex(a, b, d);

        memcpy(&state[Nb*i], d, 4 * sizeof(uint8_t));
    }
}

void buildInvS(int S[256], int INVS[256])
{
    for (int i = 0; i < 256; ++i) {
        int pos = S[i];
        INVS[pos] = i;
    }
}

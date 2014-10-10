
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "des.h"
#include "util.h"


static void sbox(int B[6], int* Si, int res[4]);
static void transpose(int* T1, int* T2);

int m_flag = 1;

/*
 * DES symmetric-key block cipher encryption of `m' (which is an array of
 * integers representing a 64-bit block of data) using IP, E, P, S{1-8}, V,
 * PC1, PC2 tables specified in arg `table' respectively. Keys for each
 * round of the DES algorithm are specified using arg `keys' and the result
 * of encryption is stored in arg `res'.
 */
void des(int m[64], int** table, int keys[16][48], int res[64])
{
    int L[32];
    int R[32];
    int Li[32];
    int Ri[32];
    int T[48];
    int B[6];

    memset(L, 0, 32 * sizeof(int));
    memset(R, 0, 32 * sizeof(int));
    memset(Li, 0, 32 * sizeof(int));
    memset(Ri, 0, 32 * sizeof(int));
    memset(T, 0, 48 * sizeof(int));
    memset(B, 0, 6 * sizeof(int));

    int* IP = table[0];
    int* E  = table[1];
    int* P  = table[2];
    int* S1 = table[3];
    int* S2 = table[4];
    int* S3 = table[5];
    int* S4 = table[6];
    int* S5 = table[7];
    int* S6 = table[8];
    int* S7 = table[9];
    int* S8 = table[10];

    /* setup L and R using IP table */
    for (int i = 0; i < 32; ++i) {
        int pos = IP[i] - 1;  /* decrement by 1, DES tables are 1-based */
        L[i] = m[pos];
    }
    for (int i = 32; i < 64; ++i) {
        int pos = IP[i] - 1;  /* decrement by 1, DES tables are 1-based */
        R[i-32] = m[pos];
    }

    int LR[64];
    memset(LR, 0, 64 * sizeof(int));

    /* concatenate L and R */
    memcpy(&LR[0], L, 32 * sizeof(int));
    memcpy(&LR[32], R, 32 * sizeof(int));

    uint8_t lrstr[64/4+1];
    memset(lrstr, '\0', sizeof(lrstr));

    if (m_flag) {
        hexstring(LR, 64, lrstr);
        fprintf(stderr, "(L0,R0)=%s\n", lrstr);
    }

    /* perform 16 des rounds */
    for (int i = 0; i < 16; ++i) {

        /* compute Li and Ri from prev values */
        memcpy(Li, R, 32 * sizeof(int));
        memcpy(Ri, L, 32 * sizeof(int));

        /* expand R from 32 to 48 bits */
        for (int j = 0; j < 48; ++j) {
            int pos = E[j] - 1;  /* decrement by 1, DES tables are 1-based */
            T[j] = R[pos];
        }

        /* xor T with this round's key */
        for (int j = 0; j < 48; ++j) {
            T[j] = T[j] ^ keys[i][j];
        }

        int TT[32];
        memset(TT, 0, 32 * sizeof(int));

        /* map every 8 bits to 6 using S-boxes */
        for (int j = 0, u = 0; j < 48; j += 6, u += 4) {
            memcpy(B, &T[j], 6 * sizeof(int));

            int t[4];
            memset(t, 0, 4 * sizeof(int));

            int k = j / 6 + 1;

            switch (k) {
                case 1: sbox(B, S1, t); break;
                case 2: sbox(B, S2, t); break;
                case 3: sbox(B, S3, t); break;
                case 4: sbox(B, S4, t); break;
                case 5: sbox(B, S5, t); break;
                case 6: sbox(B, S6, t); break;
                case 7: sbox(B, S7, t); break;
                case 8: sbox(B, S8, t); break;
                default:
                    fprintf(stderr, "Error: invalid S-box index: %d\n", k);
                    exit(-1);
            }

            memcpy(&TT[u], &t[0], 4 * sizeof(int));
        }

        int TTT[32];
        memset(TTT, 0, 32 * sizeof(int));

        /* transform TT to TTT (L) using P table */
        for (int j = 0; j < 32; ++j) {
            int pos = P[j] - 1;  /* decrement by 1, DES tables are 1-based */
            TTT[j] = TT[pos];
        }

        /* Ri has L from previous round, so xor with self and TTT */
        for (int j = 0; j < 32; ++j) {
            Ri[j] = Ri[j] ^ TTT[j];
        }

        memcpy(&LR[0], Li, 32 * sizeof(int));
        memcpy(&LR[32], Ri, 32 * sizeof(int));

        memset(lrstr, '\0', sizeof(lrstr));

        if (m_flag) {
            hexstring(LR, 64, lrstr);
            fprintf(stderr, "(L%d,R%d)=%s\n", i+1, i+1, lrstr);
        }

        /* copy to get ready for next round */
        memcpy(L, R, 32 * sizeof(int));
        memcpy(R, Ri, 32 * sizeof(int));
    }

    /* exchange final blocks */
    memcpy(&LR[0], Ri, 32 * sizeof(int));
    memcpy(&LR[32], Li, 32 * sizeof(int));

    int IPINV[64];
    memset(IPINV, 0, 64 * sizeof(int));

    /* transpose IP table to get inverse */
    transpose(IP, IPINV);

    /* copy inverse values to final result */
    for (int i = 0; i < 64; ++i) {
        int pos = IPINV[i] - 1;  /* decrement by 1, DES tables are 1-based */
        res[i] = LR[pos];
    }

    m_flag = 0;
}

void sbox(int B[6], int* Si, int res[4])
{
    int r = 2 * B[0] + B[5];
    int c = 0x8 * B[1] + 0x4 * B[2] + 0x2 * B[3] + 0x1 * B[4];

    int i = 16 * r + c;

    res[0] = 0x08 & Si[i] ? 1 : 0;
    res[1] = 0x04 & Si[i] ? 1 : 0;
    res[2] = 0x02 & Si[i] ? 1 : 0;
    res[3] = 0x01 & Si[i] ? 1 : 0;
}

void transpose(int* T1, int* T2)
{
    for (int i = 0; i < 64; ++i) {
        int pos = T1[i];
        /* decrement, then increment by 1, DES tables are 1-based */
        T2[pos-1] = i + 1;
    }
}


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tablecheck.h"

#define TABLE_NUM_LINES  14
#define MAX_ALLOC_SIZE  1024 * 1024


static int tablepos(const char* key);
static void checktsizes(int* table);
static void checkvalues(int** table);

int** tablecheck(const char* tablefile)
{
    FILE* fp = NULL;
    int** table = NULL;

    if (!(fp = fopen(tablefile, "rb"))) {
        char errorMsg[CHAR_BUF_LEN];
        sprintf(errorMsg, "Error opening %s", tablefile);
        perror(errorMsg);
        exit(-1);
    }

    int c = EOF;
    int nChars = 0;
    char* input = NULL;

    while ((c = fgetc(fp)) != EOF) {
        nChars += 1;
    }

    if (nChars <= MAX_ALLOC_SIZE) {
        input = malloc(nChars * sizeof(char));
        if (!input) {
            fprintf(stderr, "Error: couldn't alloc input, bailing.\n");
            exit(-1);
        }

        rewind(fp);
        fread(input, 1, nChars, fp);
    }
    else {
        fprintf(stderr, "Error: input size greater than MAX_ALLOC_SIZE, bailing.\n");
        exit(-1);
    }

    if (fp) {
        fclose(fp);
    }

    int tsizes[TABLE_NUM_LINES]; /* IP, E, P, S{1-8}, V, PC{1,2} */
    memset(tsizes, 0, TABLE_NUM_LINES * sizeof(int));

    int kpos = 0;
    int nCommas = 0;

    char key[CHAR_BUF_LEN];
    char val[CHAR_BUF_LEN];

    memset(key, '\0', CHAR_BUF_LEN * sizeof(char));
    memset(val, '\0', CHAR_BUF_LEN * sizeof(char));

    for (int i = 0; i < strlen(input); ++i) {
        if (input[i] == '=') {
            memset(key, '\0', CHAR_BUF_LEN * sizeof(char));
            memcpy(key, &input[kpos], i-kpos);
        }
        else if (input[i] == ',') {
            nCommas += 1;
        }

        if (input[i] == '\n') {
            if (tablepos(key) >= 0 && tablepos(key) < TABLE_NUM_LINES) {
                tsizes[tablepos(key)] = nCommas + 1;
            }
            else {
                fprintf(stderr, "Error: invalid tablefile, bailing.\n");
                exit(-1);
            }

            kpos = i + 1;
            nCommas = 0;
        }
    }

    checktsizes(tsizes);

    table = malloc(TABLE_NUM_LINES * sizeof(int*));
    if (!table) {
        fprintf(stderr, "Error: couldn't alloc table, bailing.\n");
        exit(-1);
    }
    for (int j = 0; j < TABLE_NUM_LINES; ++j) {
        table[j] = malloc(tsizes[j] * sizeof(int));
    }

    kpos = 0;
    int pos = 0;
    int vpos = 0;

    for (int i = 0; i < strlen(input); ++i) {
        if (input[i] == '=') {
            memset(key, '\0', CHAR_BUF_LEN * sizeof(char));
            memcpy(key, &input[kpos], i-kpos);
            vpos = i + 1;
        }

        if (input[i] == ',' || input[i] == '\n' || i == strlen(input)-1) {
            memset(val, '\0', CHAR_BUF_LEN * sizeof(char));
            memcpy(val, &input[vpos], i-vpos);
            if (atoi(val) == 0 && strcmp(val, "0") != 0) {
                fprintf(stderr, "Error: invalid tablefile, bailing.\n");
                exit(-1);
            }
            else {
                table[tablepos(key)][pos++] = atoi(val);
            }
            vpos = i + 1;
        }

        if (input[i] == '\n') {
            pos = 0;
            kpos = i + 1;
        }
    }

    if (input) {
        free(input);
    }

    checkvalues(table);

    return table;
}

int tablepos(const char* key)
{
    if (!strcmp(key, "IP"))  return  0;
    if (!strcmp(key, "E"))   return  1;
    if (!strcmp(key, "P"))   return  2;
    if (!strcmp(key, "S1"))  return  3;
    if (!strcmp(key, "S2"))  return  4;
    if (!strcmp(key, "S3"))  return  5;
    if (!strcmp(key, "S4"))  return  6;
    if (!strcmp(key, "S5"))  return  7;
    if (!strcmp(key, "S6"))  return  8;
    if (!strcmp(key, "S7"))  return  9;
    if (!strcmp(key, "S8"))  return 10;
    if (!strcmp(key, "V"))   return 11;
    if (!strcmp(key, "PC1")) return 12;
    if (!strcmp(key, "PC2")) return 13;

    return -1;
}

void checktsizes(int* tsizes)
{
    for (int i = 0; i < 14; ++i) {
        switch (i) {
            case 0:
                if (tsizes[i] != 64) {
                    fprintf(stderr, "Error: IP wrong tablesize.\n");
                    exit(-1);
                }
                break;
            case 1:
                if (tsizes[i] != 48) {
                    fprintf(stderr, "Error: E wrong tablesize.\n");
                    exit(-1);
                }
                break;
            case 2:
                if (tsizes[i] != 32) {
                    fprintf(stderr, "Error: P wrong tablesize.\n");
                    exit(-1);
                }
                break;
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
                if (tsizes[i] != 64) {
                    fprintf(stderr, "Error: S%d wrong tablesize.\n", i-2);
                    exit(-1);
                }
                break;
            case 11:
                if (tsizes[i] != 16) {
                    fprintf(stderr, "Error: V wrong tablesize.\n");
                    exit(-1);
                }
                break;
            case 12:
                if (tsizes[i] != 56) {
                    fprintf(stderr, "Error: PC1 wrong tablesize.\n");
                    exit(-1);
                }
                break;
            case 13:
                if (tsizes[i] != 48) {
                    fprintf(stderr, "Error: PC2 wrong tablesize.\n");
                    exit(-1);
                }
                break;
            default:
                fprintf(stderr, "Error: wrong index, bailing.\n");
                exit(-1);
        }
    }
}

void checkvalues(int** table)
{
    int IP[64];
    int E[48];
    int P[32];
    int S[16];
    int V[16];
    int PC1[56];
    int PC2[48];

    memset(IP,  0, 64 * sizeof(int));
    memset(E,   0, 48 * sizeof(int));
    memset(P,   0, 32 * sizeof(int));
    memset(S,   0, 16 * sizeof(int));
    memset(V,   0, 16 * sizeof(int));
    memset(PC1, 0, 56 * sizeof(int));
    memset(PC2, 0, 48 * sizeof(int));

    for (int i = 0; i < 14; ++i) {
        switch (i) {
            case 0:
                for (int j = 0; j < 64; ++j) {
                    int pos = table[i][j] - 1;
                    if (pos >= 0 && pos < 64) {
                        IP[pos] += 1;
                    }
                }
                for (int j = 0; j < 64; ++j) {
                    if (IP[j] != 1) {
                        fprintf(stderr, "Error: bad IP table, bailing.\n");
                        exit(-1);
                    }
                }
                break;
            case 1:
                for (int j = 0; j < 48; ++j) {
                    int pos = table[i][j] - 1;
                    if (pos >= 0 && pos < 32) {
                        E[pos] += 1;
                    }
                }
                for (int j = 0; j < 32; ++j) {
                    if (E[j] != 1 && E[j] != 2) {
                        fprintf(stderr, "Error: bad E table, bailing.\n");
                        exit(-1);
                    }
                }
                break;
            case 2:
                for (int j = 0; j < 32; ++j) {
                    int pos = table[i][j] - 1;
                    if (pos >= 0 && pos < 32) {
                        P[pos] += 1;
                    }
                }
                for (int j = 0; j < 32; ++j) {
                    if (P[j] != 1) {
                        fprintf(stderr, "Error: bad P table, bailing.\n");
                        exit(-1);
                    }
                }
                break;
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
                memset(S, 0, 16 * sizeof(int));
                for (int j = 0; j < 64; ++j) {
                    int pos = table[i][j];
                    if (pos >= 0 && pos < 16) {
                        S[pos] += 1;
                    }
                }
                for (int j = 0; j < 16; ++j) {
                    if (S[j] != 4) {
                        fprintf(stderr, "Error: bad S%d table, bailing.\n", i-2);
                        exit(-1);
                    }
                }
                break;
            case 11: {
                int n1 = 0;
                int n2 = 0;
                for (int j = 0; j < 16; ++j) {
                    if (table[i][j] == 1) {
                        n1 += 1;
                    }
                    if (table[i][j] == 2) {
                        n2 += 1;
                    }
                }
                for (int j = 0; j < 16; ++j) {
                    if (n1 != 4 || n2 != 12) {
                        fprintf(stderr, "Error: bad V table, bailing.\n");
                        exit(-1);
                    }
                }
                break;
            }
            case 12:
                for (int j = 0; j < 56; ++j) {
                    int pos = table[i][j] - 1;
                    if ((pos+1) % 8 == 0) {
                        fprintf(stderr, "Error: bad PC1 table, bailing.\n");
                        exit(-1);
                    }
                    if (pos >= 0 && pos < 64) {
                        PC1[pos] += 1;
                    }
                }
                for (int j = 0; j < 56; ++j) {
                    if (PC1[j] > 1) {
                        fprintf(stderr, "Error: bad PC1 table, bailing.\n");
                        exit(-1);
                    }
                }
                break;
            case 13:
                for (int j = 0; j < 48; ++j) {
                    int pos = table[i][j] - 1;
                    if (pos >= 0 && pos < 56) {
                        PC2[pos] += 1;
                    }
                }
                for (int j = 0; j < 48; ++j) {
                    if (PC2[j] > 1) {
                        fprintf(stderr, "Error: bad PC2 table, bailing.\n");
                        exit(-1);
                    }
                }
                break;
        }
    }
}

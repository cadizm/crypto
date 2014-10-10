
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tablecheck.h"
#include "util.h"

#define TABLE_NUM_LINES  3
#define MAX_ALLOC_SIZE  1024 * 1024


static int tablepos(const char* key);
static void convertvalues(char** table, int** res);

void tablecheck(const char* tablefile, int** res)
{
    FILE* fp = NULL;
    char** table = malloc(3 * sizeof(char*));

    if (!(fp = fopen(tablefile, "rb"))) {
        char errorMsg[CHAR_BUF_LEN];
        sprintf(errorMsg, "Error opening %s", tablefile);
        perror(errorMsg);
        exit(-1);
    }

    int c = EOF;
    int nChars = 0;
    int nNewlines = 0;
    char* input = NULL;

    while ((c = fgetc(fp)) != EOF) {
        nChars += 1;
        if (c == '\n') {
            nNewlines += 1;
        }
    }

    if (nNewlines != TABLE_NUM_LINES) {
        fprintf(stderr, "Error: invalid tablefile, bailing.\n");
        exit(-1);
    }

    if (nChars <= MAX_ALLOC_SIZE) {
        input = malloc(nChars * sizeof(char));
        if (!input) {
            fprintf(stderr, "Error: couldn't alloc input, bailing.\n");
            exit(-1);
        }

        rewind(fp);
        fread(input, 1, nChars-1, fp);
    }
    else {
        fprintf(stderr, "Error: input size greater than MAX_ALLOC_SIZE, bailing.\n");
        exit(-1);
    }

    if (fp) {
        fclose(fp);
        fp = NULL;
    }

    char* str = strtok(input, "\n");

    for (int i = 0; i < 3; ++i) {
        int eqpos = -1;
        for (int j = 0; j < strlen(str); ++j) {
            if (str[j] == '=') {
                eqpos = j;
                break;
            }
        }
        if (eqpos == -1 || eqpos > 4) {
            fprintf(stderr, "Error: invalid input, `=' not found, bailing.\n");
            exit(-1);
        }
        else {
            char key[5];
            memset(key, '\0', sizeof(key));
            memcpy(key, str, eqpos);
            int pos = tablepos(key);
            if (pos == -1) {
                fprintf(stderr, "Error: invalid input, bad key, bailing.\n");
                exit(-1);
            }
            else {
                switch (pos) {
                    case 0:
                        if (strlen(str) != 514) {
                            fprintf(stderr, "Error: invalid input, bad `S' value, bailing.\n");
                            exit(-1);
                        }
                        else {
                            table[0] = malloc(512 * sizeof(char));
                            memcpy(table[0], &str[2], 512 * sizeof(char));
                        }
                        break;
                    case 1:
                        if (strlen(str) != 10) {
                            fprintf(stderr, "Error: invalid input, bad `P' value, bailing.\n");
                            exit(-1);
                        }
                        else {
                            table[1] = malloc(8 * sizeof(char));
                            memcpy(table[1], &str[2], 8 * sizeof(char));
                        }
                        break;
                    case 2:
                        if (strlen(str) != 13) {
                            fprintf(stderr, "Error: invalid input, bad `INVP' value, bailing.\n");
                            exit(-1);
                        }
                        else {
                            table[2] = malloc(8 * sizeof(char));
                            memcpy(table[2], &str[5], 8 * sizeof(char));
                        }
                        break;
                }
            }
        }

        str = strtok(NULL, "\n");
    }

    convertvalues(table, res);

    uint8_t a[4];
    uint8_t b[4];
    uint8_t d[4];

    memset(a, 0, sizeof(a));
    memset(b, 0, sizeof(b));
    memset(d, 0, sizeof(d));

    a[0] = res[1][3];
    a[1] = res[1][2];
    a[2] = res[1][1];
    a[3] = res[1][0];

    b[0] = res[2][3];
    b[1] = res[2][2];
    b[2] = res[2][1];
    b[3] = res[2][0];

    circlex(a, b, d);

    if (d[3] != 0 || d[2] != 0 || d[1] != 0 || d[0] != 1) {
        fprintf(stderr, "Error: invalid input, `P' or `INVP', bailing.\n");
        exit(-1);
    }

    if (table[0]) {
        free(table[0]);
    }
    if (table[1]) {
        free(table[1]);
    }
    if (table[2]) {
        free(table[2]);
    }
    if (table) {
        free(table);
    }
    if (input) {
        free(input);
    }
}

int tablepos(const char* key)
{
    if (!strcmp(key, "S"))    return  0;
    if (!strcmp(key, "P"))    return  1;
    if (!strcmp(key, "INVP")) return  2;

    return -1;
}

void convertvalues(char** table, int** res)
{
    char buf[3];
    memset(buf, '\0', sizeof(buf));

    int verify[256];
    memset(verify, 0, sizeof(verify));

    for (int k = 0; k < 3; ++k) {
        for (int i = 0, j = 0; i < strlen(table[k]); i += 2, ++j) {
            memcpy(buf, &table[k][i], 2 * sizeof(char));
            res[k][j] = hex2dec(buf);
        }
        if (k == 0) {
            for (int i = 0; i < 256; ++i) {
                int d = res[k][i];
                verify[d] += 1;
            }
            for (int i = 0; i < 256; ++i) {
                if (verify[i] != 1) {
                    fprintf(stderr, "Error: invalid tablefile, bad `S' value, bailing.\n");
                    exit(-1);
                }
            }
        }
    }
}

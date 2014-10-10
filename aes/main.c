
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>

#include "tablecheck.h"
#include "modprod.h"
#include "inverse.h"
#include "keyexpand.h"
#include "encrypt.h"
#include "decrypt.h"


static void usage();

void usage()
{
    printf("Usage:\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n",
        "hw6 tablecheck -t=tablefile",
        "hw6 modprod -p1=poly1 -p2=poly2",
        "hw6 keyexpand -k=key -t=tablefile",
        "hw6 encrypt -k=key -t=tablefile [file]",
        "hw6 decrypt -k=key -t=tablefile [file]",
        "hw6 inverse -p=poly");
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        usage();
        exit(-1);
    }

    if (strcmp(argv[1], "tablecheck") && strcmp(argv[1], "modprod") &&
        strcmp(argv[1], "keyexpand") && strcmp(argv[1], "encrypt") &&
        strcmp(argv[1], "decrypt") && strcmp(argv[1], "inverse")) {
        usage();
        exit(-1);
    }

    if (!strcmp(argv[1], "tablecheck")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char tablefile[CHAR_BUF_LEN];
        memset(tablefile, '\0', CHAR_BUF_LEN * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"t", required_argument, 0, 't'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "t:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind != 3) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 't': {
                    strcpy(tablefile, optarg);
                    break;
                }

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(tablefile) > 0) {
            int S[256];
            int P[4];
            int INVP[4];

            memset(&S[0], 0, sizeof(S));
            memset(&P[0], 0, sizeof(P));
            memset(&INVP[0], 0, sizeof(INVP));

            int* table[3];

            table[0] = &S[0];
            table[1] = &P[0];
            table[2] = &INVP[0];

            tablecheck(tablefile, table);
        }
        else {
            usage();
            exit(-1);
        }
    }

    else if (!strcmp(argv[1], "modprod")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char poly1[CHAR_BUF_LEN];
        char poly2[CHAR_BUF_LEN];

        memset(poly1, '\0', CHAR_BUF_LEN * sizeof(char));
        memset(poly2, '\0', CHAR_BUF_LEN * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"p1", required_argument, 0, '1'},
                {"p2", required_argument, 0, '2'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "p1:p2:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind != 4) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case '1': {
                    strcpy(poly1, optarg);
                    break;
                }

                case '2': {
                    strcpy(poly2, optarg);
                    break;
                }

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(poly1) > 0 && strlen(poly2) > 0) {
            modprod(poly1, poly2);
        }
        else {
            usage();
            exit(-1);
        }
    }

    else if (!strcmp(argv[1], "keyexpand")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char key[32+1];
        char tablefile[CHAR_BUF_LEN];

        memset(key, '\0', (32+1) * sizeof(char));
        memset(tablefile, '\0', CHAR_BUF_LEN * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"k", required_argument, 0, 'k'},
                {"t", required_argument, 0, 't'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "k:t:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind != 4) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 'k':
                    strcpy(key, optarg);
                    break;

                case 't':
                    strcpy(tablefile, optarg);
                    break;

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(key) != 32 || strlen(tablefile) < 1) {
            usage();
            exit(-1);
        }
        else {
            uint32_t w[Nb*(Nr+1)];
            memset(w, 0, Nb*(Nr+1) * sizeof(uint32_t));

            keyexpand(key, tablefile, w, 1);
        }
    }

    else if (!strcmp(argv[1], "encrypt") || !strcmp(argv[1], "decrypt")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char key[CHAR_BUF_LEN];
        char tablefile[CHAR_BUF_LEN];
        FILE* fp = NULL;

        memset(key, '\0', CHAR_BUF_LEN * sizeof(char));
        memset(tablefile, '\0', CHAR_BUF_LEN * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"k", required_argument, 0, 'k'},
                {"t", required_argument, 0, 't'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "k:t:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind < 4) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 'k':
                    strcpy(key, optarg);
                    break;

                case 't':
                    strcpy(tablefile, optarg);
                    break;

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(key) != 32 || strlen(tablefile) < 1) {
            usage();
            exit(-1);
        }

        if (optind < argc) {
            if (!(fp = fopen(argv[optind], "rb"))) {
                char errorMsg[CHAR_BUF_LEN];
                sprintf(errorMsg, "Error opening %s", argv[optind]);
                perror(errorMsg);
                exit(-1);
            }
        }
        else {
            fp = stdin;
        }

        if (!strcmp(argv[1], "encrypt")) {
            encryptaes(key, tablefile, fp);
        }
        else if (!strcmp(argv[1], "decrypt")) {
            decrypt(key, tablefile, fp);
        }
        else {
            fprintf(stderr, "Error: unknown option, bailing.\n");
            exit(-1);
        }
    }

    else if (!strcmp(argv[1], "inverse")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char poly[CHAR_BUF_LEN];
        memset(poly, '\0', CHAR_BUF_LEN * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"p", required_argument, 0, 'p'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "p:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind != 3) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 'p': {
                    strcpy(poly, optarg);
                    break;
                }

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(poly) == 8) {
            inverse(poly);
        }
        else {
            fprintf(stderr, "Error: bad polynomial, bailing.\n");
            exit(-1);
        }
    }

    return 0;
}

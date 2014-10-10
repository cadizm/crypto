
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>

#include "keygen.h"
#include "crypt.h"
#include "invkey.h"
#include "histo.h"
#include "solve.h"


static void usage();

void usage()
{
    printf("Usage:\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n",
        "hw4 keygen -t=period",
        "hw4 crypt -k=keyfile [file]",
        "hw4 invkey keyfile",
        "hw4 histo -t=period -i=which [file]",
        "hw4 solve -l=max_t file");
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        usage();
        exit(-1);
    }

    if (strcmp(argv[1], "keygen") && strcmp(argv[1], "crypt") &&
        strcmp(argv[1], "invkey") && strcmp(argv[1], "histo") &&
        strcmp(argv[1], "solve")) {
        usage();
        exit(-1);
    }

    if (!strcmp(argv[1], "keygen")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        int nPeriod = 0;

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
                    for (int i = 0; i < strlen(optarg); ++i) {
                        if (!isdigit(optarg[i])) {
                            fprintf(stderr, "Error: argument `%s' is invalid\n", optarg);
                            exit(-1);
                        }
                    }
                    nPeriod = atoi(optarg);
                    break;
                }

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (nPeriod > 0) {
            keygen(nPeriod);
        }
        else {
            usage();
            exit(-1);
        }
    }

    else if (!strcmp(argv[1], "crypt")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char keyfile[CHAR_BUF_LEN];
        FILE* fp = NULL;

        memset(keyfile, '\0', CHAR_BUF_LEN * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"k", required_argument, 0, 'k'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "k:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind < 3) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 'k':
                    strcpy(keyfile, optarg);
                    break;

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(keyfile) < 1) {
            usage();
            exit(-1);
        }

        if (optind < argc) {
            if (!(fp = fopen(argv[optind], "r"))) {
                char errorMsg[CHAR_BUF_LEN];
                sprintf(errorMsg, "Error opening %s", argv[optind]);
                perror(errorMsg);
                exit(-1);
            }
        }
        else {
            fp = stdin;
        }

        crypt_(keyfile, fp);
    }

    else if (!strcmp(argv[1], "invkey")) {
        if (argc != 3) {
            usage();
            exit(-1);
        }

        invkey(argv[2]);
    }

    else if (!strcmp(argv[1], "histo")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        int nPeriod = 0;
        int nWhich = 0;
        FILE* fp = NULL;

        while (1) {
            struct option long_options[] = {
                {"t", required_argument, 0, 't'},
                {"i", required_argument, 0, 'i'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "t:i:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind < 4) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 't':
                    for (int i = 0; i < strlen(optarg); ++i) {
                        if (!isdigit(optarg[i])) {
                            fprintf(stderr, "Error: argument `%s' is invalid\n", optarg);
                            exit(-1);
                        }
                    }
                    nPeriod = atoi(optarg);
                    break;

                case 'i':
                    for (int i = 0; i < strlen(optarg); ++i) {
                        if (!isdigit(optarg[i])) {
                            fprintf(stderr, "Error: argument `%s' is invalid\n", optarg);
                            exit(-1);
                        }
                    }
                    nWhich = atoi(optarg);
                    break;

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (nPeriod < 1 || nWhich < 1) {
            usage();
            exit(-1);
        }

        if (optind < argc) {
            if (!(fp = fopen(argv[optind], "r"))) {
                char errorMsg[CHAR_BUF_LEN];
                sprintf(errorMsg, "Error opening %s", argv[optind]);
                perror(errorMsg);
                exit(-1);
            }
        }
        else {
            fp = stdin;
        }

        histo(nPeriod, nWhich, fp);
    }

    else if (!strcmp(argv[1], "solve")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        int nMax_t = 0;
        FILE* fp = NULL;

        if (argc != 4) {
            usage();
            exit(-1);
        }

        while (1) {
            struct option long_options[] = {
                {"l", required_argument, 0, 'l'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "l:",
                    long_options, &option_index);

            if (c == -1) {
                break;
            }

            switch (c) {
                case 'l':
                    for (int i = 0; i < strlen(optarg); ++i) {
                        if (!isdigit(optarg[i])) {
                            fprintf(stderr, "Error: argument `%s' is invalid\n", optarg);
                            exit(-1);
                        }
                    }
                    nMax_t = atoi(optarg);
                    break;

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (nMax_t < 1) {
            usage();
            exit(-1);
        }

        if (!(fp = fopen(argv[optind], "r"))) {
            char errorMsg[CHAR_BUF_LEN];
            sprintf(errorMsg, "Error opening %s", argv[optind]);
            perror(errorMsg);
            exit(-1);
        }

        solve(nMax_t, fp);
    }

    return 0;
}


#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>

#include "tablecheck.h"
#include "encrypt.h"
#include "decrypt.h"
#include "encrypt3.h"
#include "decrypt3.h"


static void usage();

void usage()
{
    printf("Usage:\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n",
        "hw5 tablecheck -t=tablefile",
        "hw5 encrypt -k=key -t=tablefile [file]",
        "hw5 decrypt -k=key -t=tablefile [file]",
        "hw5 encrypt3 -k=key3 -t=tablefile [file]",
        "hw5 decrypt3 -k=key3 -t=tablefile [file]");
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        usage();
        exit(-1);
    }

    if (strcmp(argv[1], "tablecheck") && strcmp(argv[1], "encrypt") &&
        strcmp(argv[1], "decrypt") && strcmp(argv[1], "encrypt3") &&
        strcmp(argv[1], "decrypt3")) {
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
            tablecheck(tablefile);
        }
        else {
            usage();
            exit(-1);
        }
    }

    else {
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

        if (strlen(key) < 1 || strlen(tablefile) < 1) {
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
            encryptdes(key, tablefile, fp);
        }
        else if (!strcmp(argv[1], "decrypt")) {
            decrypt(key, tablefile, fp);
        }
        else if (!strcmp(argv[1], "encrypt3")) {
            encrypt3(key, tablefile, fp);
        }
        else if (!strcmp(argv[1], "decrypt3")) {
            decrypt3(key, tablefile, fp);
        }
        else {
            fprintf(stderr, "Error: unknown option, bailing.\n");
            exit(-1);
        }
    }

    return 0;
}

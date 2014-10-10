
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>

#include "stream.h"
#include "encrypt.h"
#include "merge.h"
#include "decrypt.h"


static void usage();

void usage()
{
    printf("Usage:\t%s\n\t%s\n\t%s\n\t%s\n",
        "hw2 stream -p=pphrase -l=len",
        "hw2 encrypt -p=pphrase -out=name [pbmfile]",
        "hw2 merge pbmfile1 pbmfile2",
        "hw2 decrypt [pbmfile]");
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage();
        exit(-1);
    }

    if (strcmp(argv[1], "stream") && strcmp(argv[1], "encrypt") &&
        strcmp(argv[1], "merge") && strcmp(argv[1], "decrypt")) {
        usage();
        exit(-1);
    }

    if (!strcmp(argv[1], "stream")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        int len = 0;
        char pphrase[CHAR_BUF];
        memset(pphrase, '\0', CHAR_BUF * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"p", required_argument, 0, 'p'},
                {"l", required_argument, 0, 'l'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "p:l:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind != 4) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 'p':
                    strcpy(pphrase, optarg);
                    break;

                case 'l': {
                    for (int i = 0; i < strlen(optarg); ++i) {
                        if (!isdigit(optarg[i])) {
                            fprintf(stderr, "Error: argument `%s' is invalid\n", optarg);
                            exit(-1);
                        }
                    }
                    len = atoi(optarg);
                    break;
                }

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (len > 0 && strlen(pphrase) > 0) {
            stream(pphrase, len, 1);
        }
        else {
            usage();
            exit(-1);
        }
    }

    else if (!strcmp(argv[1], "encrypt")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char pphrase[CHAR_BUF];
        char outfile[CHAR_BUF];
        FILE* fp = NULL;

        memset(pphrase, '\0', CHAR_BUF * sizeof(char));
        memset(outfile, '\0', CHAR_BUF * sizeof(char));

        while (1) {
            static struct option long_options[] = {
                {"p", required_argument, 0, 'p'},
                {"out", required_argument, 0, 'o'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "p:o:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind < 4) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 'p':
                    strcpy(pphrase, optarg);
                    break;

                case 'o':
                    strcpy(outfile, optarg);
                    break;

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(pphrase) < 1 || strlen(outfile) < 1) {
            usage();
            exit(-1);
        }

        if (optind < argc) {
            if (!(fp = fopen(argv[optind], "rb"))) {
                char errorMsg[CHAR_BUF];
                sprintf(errorMsg, "Error opening %s", argv[optind]);
                perror(errorMsg);
                exit(-1);
            }
        }
        else {
            fp = stdin;
        }

        ss_encrypt(pphrase, outfile, fp);
    }

    else if (!strcmp(argv[1], "merge")) {
        if (argc != 4) {
            usage();
            exit(-1);
        }
        merge(argv[2], argv[3]);
    }

    else if (!strcmp(argv[1], "decrypt")) {
        if (argc != 2 && argc != 3) {
            usage();
            exit(-1);
        }
        if (argc == 2) {
            decrypt(stdin);
        }
        else {
            FILE* fp = NULL;

            if (!(fp = fopen(argv[2], "rb"))) {
                char errorMsg[CHAR_BUF];
                sprintf(errorMsg, "Error opening %s", argv[2]);
                perror(errorMsg);
                exit(-1);
            }

            decrypt(fp);
        }
    }

    return 0;
}

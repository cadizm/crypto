
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <openssl/bn.h>

#include "defines.h"
#include "primes.h"
#include "trialdiv.h"
#include "millerrabin.h"
#include "rndsearch.h"
#include "maurer.h"


static void usage();

void usage()
{
    printf("Usage:\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n",
        "hw7 primes -n=maxval",
        "hw7 trialdiv -n=number -p=primesfile",
        "hw7 millerrabin -n=number -t=maxitr -p=primesfile",
        "hw7 rndsearch -k=numbits -t=maxitr -p=primesfile -r=rndfile",
        "hw7 maurer -k=numbits -p=primesfile -r=rndfile");
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        usage();
        exit(-1);
    }

    if (strcmp(argv[1], "primes") && strcmp(argv[1], "trialdiv") &&
        strcmp(argv[1], "millerrabin") && strcmp(argv[1], "rndsearch") &&
        strcmp(argv[1], "maurer")) {
        usage();
        exit(-1);
    }

    if (!strcmp(argv[1], "primes")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char strMaxval[CHAR_BUF_LEN];
        memset(strMaxval, '\0', CHAR_BUF_LEN * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"n", required_argument, 0, 'n'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "n:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind != 3) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 'n': {
                    strcpy(strMaxval, optarg);
                    break;
                }

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(strMaxval) > 0 && strlen(strMaxval) < 9) {
            for (int i = 0; i < strlen(strMaxval); ++i) {
                if (!isdigit(strMaxval[i])){
                    fprintf(stderr, "Error: invalid input (%s), bailing.\n", strMaxval);
                    exit(-1);
                }
            }
            uint64_t nlong = atol(strMaxval);
            if (nlong > 0 && nlong <= _2Pow24) {
                int maxval = (uint32_t)nlong;
                primes(maxval);
            }
            else {
                fprintf(stderr, "Error: invalid range (%s), bailing.\n", strMaxval);
                exit(-1);
            }
        }
        else {
            usage();
            exit(-1);
        }
    }

    else if (!strcmp(argv[1], "trialdiv")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char strNumber[CHAR_BUF_LEN];
        char primesfile[CHAR_BUF_LEN];

        memset(strNumber, '\0', CHAR_BUF_LEN * sizeof(char));
        memset(primesfile, '\0', CHAR_BUF_LEN * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"n", required_argument, 0, 'n'},
                {"p", required_argument, 0, 'p'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "n:p:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind != 4) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 'n': {
                    strcpy(strNumber, optarg);
                    break;
                }

                case 'p': {
                    strcpy(primesfile, optarg);
                    break;
                }

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(strNumber) > 0 && strlen(primesfile) > 0) {
            for (int i = 0; i < strlen(strNumber); ++i) {
                if (!isdigit(strNumber[i])){
                    fprintf(stderr, "Error: invalid input (%s), bailing.\n", strNumber);
                    exit(-1);
                }
            }

            BIGNUM* bn_num = NULL;
            FILE* fp = NULL;

            BN_dec2bn(&bn_num, strNumber);
            if (bn_num == NULL) {
                fprintf(stderr, "Error: couldn't alloc BIGNUM, bailing.\n");
                exit(-1);
            }

            if (!(fp = fopen(primesfile, "rb"))) {
                char errorMsg[CHAR_BUF_LEN];
                sprintf(errorMsg, "Error opening %s", primesfile);
                perror(errorMsg);
                exit(-1);
            }

            trialdiv(bn_num, fp, 1);
        }
        else {
            usage();
            exit(-1);
        }
    }

    else if (!strcmp(argv[1], "millerrabin")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char strNumber[CHAR_BUF_LEN];
        char strMaxitr[CHAR_BUF_LEN];
        char primesfile[CHAR_BUF_LEN];

        memset(strNumber, '\0', CHAR_BUF_LEN * sizeof(char));
        memset(strMaxitr, '\0', CHAR_BUF_LEN * sizeof(char));
        memset(primesfile, '\0', CHAR_BUF_LEN * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"n", required_argument, 0, 'n'},
                {"t", required_argument, 0, 't'},
                {"p", required_argument, 0, 'p'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "n:t:p:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind != 5) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 'n':
                    strcpy(strNumber, optarg);
                    break;

                case 't':
                    strcpy(strMaxitr, optarg);
                    break;

                case 'p':
                    strcpy(primesfile, optarg);
                    break;

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(strNumber) > 0 && strlen(strMaxitr) > 0 &&
                strlen(primesfile) > 0) {
            for (int i = 0; i < strlen(strNumber); ++i) {
                if (!isdigit(strNumber[i])){
                    fprintf(stderr, "Error: invalid input (%s), bailing.\n", strNumber);
                    exit(-1);
                }
            }
            for (int i = 0; i < strlen(strMaxitr); ++i) {
                if (!isdigit(strMaxitr[i])){
                    fprintf(stderr, "Error: invalid input (%s), bailing.\n", strMaxitr);
                    exit(-1);
                }
            }

            BIGNUM* bn_num = NULL;
            FILE* fp = NULL;

            BN_dec2bn(&bn_num, strNumber);
            if (bn_num == NULL) {
                fprintf(stderr, "Error: couldn't alloc BIGNUM, bailing.\n");
                exit(-1);
            }

            uint64_t maxitr = atol(strMaxitr);

            if (!(fp = fopen(primesfile, "rb"))) {
                char errorMsg[CHAR_BUF_LEN];
                sprintf(errorMsg, "Error opening %s", primesfile);
                perror(errorMsg);
                exit(-1);
            }

            if (!BN_is_zero(bn_num) && maxitr > 0) {
                millerrabin(bn_num, maxitr, fp, 0);
            }
            else {
                fprintf(stderr, "Error: invalid range (%s/%s), bailing.\n", strNumber, strMaxitr);
                exit(-1);
            }
        }
        else {
            usage();
            exit(-1);
        }
    }

    else if (!strcmp(argv[1], "rndsearch")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char strNumbits[CHAR_BUF_LEN];
        char strMaxitr[CHAR_BUF_LEN];
        char primesfile[CHAR_BUF_LEN];
        char rndfile[CHAR_BUF_LEN];

        memset(strNumbits, '\0', CHAR_BUF_LEN * sizeof(char));
        memset(strMaxitr, '\0', CHAR_BUF_LEN * sizeof(char));
        memset(primesfile, '\0', CHAR_BUF_LEN * sizeof(char));
        memset(rndfile, '\0', CHAR_BUF_LEN * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"k", required_argument, 0, 'k'},
                {"t", required_argument, 0, 't'},
                {"p", required_argument, 0, 'p'},
                {"r", required_argument, 0, 'r'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "k:t:p:r:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind != 6) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 'k':
                    strcpy(strNumbits, optarg);
                    break;

                case 't':
                    strcpy(strMaxitr, optarg);
                    break;

                case 'p':
                    strcpy(primesfile, optarg);
                    break;

                case 'r':
                    strcpy(rndfile, optarg);
                    break;

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(strNumbits) > 0 && strlen(strMaxitr) > 0 &&
            strlen(primesfile) > 0 && strlen(rndfile) > 0) {
            for (int i = 0; i < strlen(strNumbits); ++i) {
                if (!isdigit(strNumbits[i])){
                    fprintf(stderr, "Error: invalid input (%s), bailing.\n", strNumbits);
                    exit(-1);
                }
            }
            for (int i = 0; i < strlen(strMaxitr); ++i) {
                if (!isdigit(strMaxitr[i])){
                    fprintf(stderr, "Error: invalid input (%s), bailing.\n", strMaxitr);
                    exit(-1);
                }
            }

            int numbits = atoi(strNumbits);
            int maxitr = atoi(strMaxitr);

            FILE* fpPrimes = NULL;
            FILE* fpRnd = NULL;

            if (!(fpPrimes = fopen(primesfile, "rb"))) {
                char errorMsg[CHAR_BUF_LEN];
                sprintf(errorMsg, "Error opening %s", primesfile);
                perror(errorMsg);
                exit(-1);
            }

            if (!(fpRnd = fopen(rndfile, "rb"))) {
                char errorMsg[CHAR_BUF_LEN];
                sprintf(errorMsg, "Error opening %s", rndfile);
                perror(errorMsg);
                exit(-1);
            }

            if (numbits > 0 && maxitr > 0) {
                rndsearch(numbits, maxitr, fpPrimes, fpRnd);
            }
            else {
                fprintf(stderr, "Error: invalid range (%s/%s), bailing.\n", strNumbits, strMaxitr);
                exit(-1);
            }
        }
        else {
            usage();
            exit(-1);
        }
    }

    else if (!strcmp(argv[1], "maurer")) {
        optind = 2;
        int c = 0;
        int option_index = 0;

        char strNumbits[CHAR_BUF_LEN];
        char primesfile[CHAR_BUF_LEN];
        char rndfile[CHAR_BUF_LEN];

        memset(strNumbits, '\0', CHAR_BUF_LEN * sizeof(char));
        memset(primesfile, '\0', CHAR_BUF_LEN * sizeof(char));
        memset(rndfile, '\0', CHAR_BUF_LEN * sizeof(char));

        while (1) {
            struct option long_options[] = {
                {"k", required_argument, 0, 'k'},
                {"p", required_argument, 0, 'p'},
                {"r", required_argument, 0, 'r'},
                {0, 0, 0, 0}
            };

            c = getopt_long_only(argc, argv, "k:p:r:",
                    long_options, &option_index);

            if (c == -1) {
                if (optind != 5) {
                    usage();
                    exit(-1);
                }
                break;
            }

            switch (c) {
                case 'k': {
                    strcpy(strNumbits, optarg);
                    break;
                }

                case 'p': {
                    strcpy(primesfile, optarg);
                    break;
                }

                case 'r': {
                    strcpy(rndfile, optarg);
                    break;
                }

                case '?':
                    break;

                default:
                    usage();
                    exit(-1);
            }
        }

        if (strlen(strNumbits) > 0 && strlen(primesfile) > 0 &&
                strlen(rndfile) > 0) {
            for (int i = 0; i < strlen(strNumbits); ++i) {
                if (!isdigit(strNumbits[i])){
                    fprintf(stderr, "Error: invalid input (%s), bailing.\n", strNumbits);
                    exit(-1);
                }
            }

            int numbits = atoi(strNumbits);

            FILE* fpPrimes = NULL;
            FILE* fpRnd = NULL;

            if (!(fpPrimes = fopen(primesfile, "rb"))) {
                char errorMsg[CHAR_BUF_LEN];
                sprintf(errorMsg, "Error opening %s", primesfile);
                perror(errorMsg);
                exit(-1);
            }

            if (!(fpRnd = fopen(rndfile, "rb"))) {
                char errorMsg[CHAR_BUF_LEN];
                sprintf(errorMsg, "Error opening %s", rndfile);
                perror(errorMsg);
                exit(-1);
            }

            if (numbits > 0) {
                maurer(0, numbits, fpPrimes, fpRnd);
            }
            else {
                fprintf(stderr, "Error: invalid range (%s), bailing.\n", strNumbits);
                exit(-1);
            }
        }
        else {
            usage();
            exit(-1);
        }
    }

    return 0;
}

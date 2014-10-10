
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "hexdump.h"
#include "enc-base64.h"
#include "dec-base64.h"
#include "mul.h"
#include "div.h"


static void usage(const char* arg0);
static void checkargs(char** argv);

void usage(const char* arg0)
{
    printf("Usage:\t%s %s\n\t%s %s\n\t%s %s\n\t%s %s\n\t%s %s\n",
        arg0, "hexdump [file]", arg0, "enc-base64 [file]", arg0,
        "dec-base64 [file]", arg0, "mul n1 n2", arg0, "div n1 n2");
}

void checkargs(char** argv)
{
    char* n1 = argv[2];
    char* n2 = argv[3];

    for (int i = 0; i < strlen(n1); ++i) {
        if (!isdigit(n1[i])) {
            fprintf(stderr, "Error: n1 is invalid.\n");
            exit(-1);
        }
    }

    for (int i = 0; i < strlen(n2); ++i) {
        if (!isdigit(n2[i])) {
            fprintf(stderr, "Error: n2 is invalid.\n");
            exit(-1);
        }
    }

    uint64_t a = 0;
    uint64_t b = 0;

    sscanf(n1, "%llu", &a);
    sscanf(n2, "%llu", &b);

    if (a > 4294967295 || (a == 0 && strlen(n1) > 1)) {
        fprintf(stderr, "Error: n1 is out of range.\n");
        exit(-1);
    }

    if (b > 4294967295 || (b == 0 && strlen(n2) > 1)) {
        fprintf(stderr, "Error: n2 is out of range.\n");
        exit(-1);
    }
}

int main(int argc, char** argv)
{
    int nStatus = 0;

    if (argc < 2) {
        usage(argv[0]);
        exit(-1);
    }

    if (!strcmp(argv[1], "hexdump")) {
        if (argc == 2) {
            nStatus = hexdump(NULL);
        }
        else if (argc == 3) {
            nStatus = hexdump(argv[2]);
        }
        else {
            usage(argv[0]);
            exit(-1);
        }
    }
    else if (!strcmp(argv[1], "enc-base64")) {
        if (argc == 2) {
            nStatus = base64encode(NULL);
        }
        else if (argc == 3) {
            nStatus = base64encode(argv[2]);
        }
        else {
            usage(argv[0]);
            exit(-1);
        }
    }
    else if (!strcmp(argv[1], "dec-base64")) {
        if (argc == 2) {
            nStatus = base64decode(NULL);
        }
        else if (argc == 3) {
            nStatus = base64decode(argv[2]);
        }
        else {
            usage(argv[0]);
            exit(-1);
        }
    }
    else if (!strcmp(argv[1], "mul")) {
        if (argc != 4) {
            usage(argv[0]);
            exit(-1);
        }
        checkargs(argv);
        mul(argv[2], argv[3]);
    }
    else if (!strcmp(argv[1], "div")) {
        if (argc != 4) {
            usage(argv[0]);
            exit(-1);
        }
        checkargs(argv);
        div_(argv[2], argv[3]);
    }
    else {
        usage(argv[0]);
        exit(-1);
    }

    return nStatus;
}

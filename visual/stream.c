
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <openssl/md5.h>

#include "stream.h"


#define KEY_LEN_PER_PASS  8

/**
 * Create a keystream from passphrase pphrase of length keylen (in bytes).
 * if printflag is true, print the created keystream to stdout. In all
 * cases, the newly created key is returned.
 */
char* stream(const char* pphrase, int keylen, int printflag)
{
    int nBytes = 0;
    int bDone = 0;

    long len = strlen(pphrase) + 2 + MD5_DIGEST_LENGTH;
    char* s = malloc(len+1);

    if (!s) {
        char errorMsg[CHAR_BUF];
        sprintf(errorMsg, "Error allocating string `s'.");
        perror(errorMsg);
        exit(-1);
    }
    memset(s, '\0', (len+1) * sizeof(char));

    char* res = malloc(keylen);

    if (!res) {
        char errorMsg[CHAR_BUF];
        sprintf(errorMsg, "Error allocating string `res'.");
        perror(errorMsg);
        exit(-1);
    }
    memset(res, '\0', (keylen) * sizeof(char));

    char md5_buf[MD5_DIGEST_LENGTH];
    memset(md5_buf, '\0', MD5_DIGEST_LENGTH * sizeof(char));

    MD5((unsigned char*)pphrase, strlen(pphrase), (unsigned char*)md5_buf);

    int i = 0;
    int pos = 0;

    while (!bDone) {
        sprintf(&s[MD5_DIGEST_LENGTH], "%02d%s", i, pphrase);
        memcpy(s, md5_buf, MD5_DIGEST_LENGTH);
        MD5((unsigned char*)s, len, (unsigned char*)md5_buf);

        for (int j = 0; j < KEY_LEN_PER_PASS; ++j) {
            if (printflag) {
                fwrite(&md5_buf[j], 1, 1, stdout);
            }
            memcpy(&res[pos++], &md5_buf[j], 1);
            if (++nBytes == keylen) {
                bDone = 1;
                break;
            }
        }

        if (++i == 100) {
            i = 0;
        }
    }

    if (s) {
        free(s);
    }

    return res;
}

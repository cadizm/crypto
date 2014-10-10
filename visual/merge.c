
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "merge.h"
#include "pbm.h"


/*
 * Merge the contents of pbmfile1 and pbmfile2 and write output to stdout.
 */
void merge(const char* pbmfile1, const char* pbmfile2)
{
    FILE* fp1 = NULL;
    FILE* fp2 = NULL;

    if (!(fp1 = fopen(pbmfile1, "rb"))) {
        char errorMsg[CHAR_BUF];
        sprintf(errorMsg, "Error opening %s", pbmfile1);
        perror(errorMsg);
        exit(-1);
    }

    if (!(fp2 = fopen(pbmfile2, "rb"))) {
        char errorMsg[CHAR_BUF];
        sprintf(errorMsg, "Error opening %s", pbmfile2);
        perror(errorMsg);
        exit(-1);
    }

    pbm_t* pbm1 = pbm_read(fp1);
    pbm_t* pbm2 = pbm_read(fp2);

    pbm_t pbm;

    pbm.size = pbm1->size;
    pbm.pad = pbm1->pad;
    pbm.width = pbm1->width;
    pbm.height = pbm1->height;
    pbm.buf = malloc(pbm.height * (pbm.width + pbm.pad));
    memset(pbm.buf, 0, pbm.height * (pbm.width + pbm.pad));

    int i = 0;
    int j = 0;
    int pbm1Block[] = {0, 0, 0, 0};
    int pbm2Block[] = {0, 0, 0, 0};

    for (i = 0; i < pbm.height; i += 2) {
        for (j = 0; j < pbm.width; j += 2) {
            pbm1Block[0] = pbm1->buf[i * (pbm1->width + pbm1->pad) + j];
            pbm1Block[1] = pbm1->buf[i * (pbm1->width + pbm1->pad) + j + 1];
            pbm1Block[2] = pbm1->buf[(i + 1) * (pbm1->width + pbm1->pad) + j];
            pbm1Block[3] = pbm1->buf[(i + 1) * (pbm1->width + pbm1->pad) + j + 1];

            pbm2Block[0] = pbm2->buf[i * (pbm2->width + pbm2->pad) + j];
            pbm2Block[1] = pbm2->buf[i * (pbm2->width + pbm2->pad) + j + 1];
            pbm2Block[2] = pbm2->buf[(i + 1) * (pbm2->width + pbm2->pad) + j];
            pbm2Block[3] = pbm2->buf[(i + 1) * (pbm2->width + pbm2->pad) + j + 1];

            pbm.buf[i * (pbm.width + pbm.pad) + j] = pbm1Block[0] | pbm2Block[0];
            pbm.buf[i * (pbm.width + pbm.pad) + j + 1] = pbm1Block[1] | pbm2Block[1];
            pbm.buf[(i + 1) * (pbm.width + pbm.pad) + j] = pbm1Block[2] | pbm2Block[2];
            pbm.buf[(i + 1) * (pbm.width + pbm.pad) + j + 1] = pbm1Block[3] | pbm2Block[3];
        }
        for (; j < pbm.width + pbm.pad; ++j) {
        }
    }

    pbm_write(stdout, &pbm);
}

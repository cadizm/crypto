
#include <stdlib.h>
#include <string.h>

#include "decrypt.h"
#include "pbm.h"


/**
 * Decrypt input from FILE pointer fp and write output to stdout.
 */
void decrypt(FILE* fp)
{
    pbm_t* pbm1 = pbm_read(fp);

    pbm_t pbm2;

    pbm2.width = pbm1->width / 2;
    pbm2.height = pbm1->height / 2;

    if (pbm2.width % 8 == 0) {
        pbm2.pad = 0;
        pbm2.size = pbm2.height * pbm2.width / 8;
    }
    else {
        int pad = 0;
        for (int width = pbm2.width; width % 8 != 0; width++) {
            pad++;
        }
        pbm2.pad = pad;
        pbm2.size = pbm2.height * (pbm2.width / 8 + 1);
    }

    pbm2.buf = malloc(pbm2.height * (pbm2.width + pbm2.pad));
    memset(pbm2.buf, 0, pbm2.height * (pbm2.width + pbm2.pad));

    int i = 0;
    int j = 0;
    int block[] = {0, 0, 0, 0};

    int curcol = 0;
    int pixelpos = 0;

    for (i = 0; i < pbm1->height; i += 2) {
        for (j = 0; j < pbm1->width; j += 2) {
            block[0] = pbm1->buf[i * (pbm1->width + pbm1->pad) + j];
            block[1] = pbm1->buf[i * (pbm1->width + pbm1->pad) + j + 1];
            block[2] = pbm1->buf[(i + 1) * (pbm1->width + pbm1->pad) + j];
            block[3] = pbm1->buf[(i + 1) * (pbm1->width + pbm1->pad) + j + 1];

            if (curcol == pbm2.width) {
                curcol = 0;
                pixelpos += pbm2.pad;
            }

            curcol++;

            if (block[0] == 1 && block[1] == 1 && block[2] == 1 && block[3] == 1) {
                pbm2.buf[pixelpos++] = 1;
            }
            else if ((block[0] == 1 && block[1] == 0 && block[2] == 0 && block[3] == 1) ||
                     (block[0] == 0 && block[1] == 1 && block[2] == 1 && block[3] == 0)) {
                pbm2.buf[pixelpos++] = 0;
            }
            else {
                char errorMsg[CHAR_BUF];
                sprintf(errorMsg, "Error invalid input files.");
                perror(errorMsg);
                exit(-1);
            }
        }
        for (; j < pbm1->width + pbm1->pad; ++j) {
        }
    }

    pbm_write(stdout, &pbm2);
}


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "pbm.h"


static void extractBits(uint8_t c, int* pos, int* col, pbm_t* pbm);
static uint8_t packBits(int bitArr[8]);

/*
 * Read input from FILE pointer fp and return a new in memory representation
 * of the input.
 */
pbm_t* pbm_read(FILE* fp)
{
    pbm_t* pbm = (pbm_t*)malloc(sizeof(pbm_t));

    if (!pbm) {
        char errorMsg[CHAR_BUF];
        sprintf(errorMsg, "Error allocating struct pbm.");
        perror(errorMsg);
        exit(-1);
    }

    char magicNum[CHAR_BUF];
    char sWidth[CHAR_BUF];
    char sHeight[CHAR_BUF];

    memset(pbm, '\0', sizeof(pbm_t));
    memset(magicNum, '\0', CHAR_BUF * sizeof(char));
    memset(sWidth, '\0', CHAR_BUF * sizeof(char));
    memset(sHeight, '\0', CHAR_BUF * sizeof(char));

    uint8_t c;
    int i = 0;
    int nWidth = 0;
    int nHeight = 0;

    /* read magic num */
    while (!isspace((c = fgetc(fp)))) {
        magicNum[i++] = c;
        if (i >= CHAR_BUF) {
            fprintf(stderr, "Woops! Can't handle input size\n");
            exit(-1);
        }
    }
    magicNum[i] = '\0';

    /* make sure magicNum = P4 */
    if (strcmp("P4", magicNum) != 0) {
        fprintf(stderr, "Error: invalid PBM file format\n");
        exit(-1);
    }

    /* burn whitespace */
    while (isspace((c = fgetc(fp))));

    /* reset */
    i = 0;
    sWidth[i++] = c;

    /* read width */
    while (!isspace((c = fgetc(fp)))) {
        sWidth[i++] = c;
        if (i >= CHAR_BUF) {
            fprintf(stderr, "Woops! Can't handle input size\n");
            exit(-1);
        }
    }
    sWidth[i] = '\0';

    if ((nWidth = atoi(sWidth)) == 0) {
        fprintf(stderr, "Error: invalid PBM file format\n");
        exit(-1);
    }

    /* burn whitespace */
    while (isspace((c = fgetc(fp))));

    /* reset */
    i = 0;
    sHeight[i++] = c;

    /* read height */
    while (!isspace((c = fgetc(fp)))) {
        sHeight[i++] = c;
        if (i >= CHAR_BUF) {
            fprintf(stderr, "Woops! Can't handle input size\n");
            exit(-1);
        }
    }
    sHeight[i] = '\0';

    if ((nHeight = atoi(sHeight)) == 0) {
        fprintf(stderr, "Error: invalid PBM file format\n");
        exit(-1);
    }

    pbm->width = nWidth;
    pbm->height = nHeight;

    /* input is 8 packed bits per byte w/ zeroed don't care bits */
    int bytesPerRow = 0;
    if (pbm->width % 8 == 0) {
        bytesPerRow = pbm->width / 8;
    }
    else {
        bytesPerRow = pbm->width / 8 + 1;
    }

    pbm->size = bytesPerRow * 8 * pbm->height;
    pbm->pad = bytesPerRow * 8 - pbm->width;
    pbm->buf = malloc(pbm->size);

    if (!pbm->buf) {
        char errorMsg[CHAR_BUF];
        sprintf(errorMsg, "Error allocating pbm->buf.");
        perror(errorMsg);
        exit(-1);
    }
    memset(pbm->buf, 0, pbm->size);

    int nObj = 0;
    int pos = 0;
    int col = 0;

    while (!feof(fp)) {
        nObj = fread(&c, 1, 1, fp);
        if (nObj) {
            extractBits(c, &pos, &col, pbm);
        }
    }

    return pbm;
}

/*
 * Write the contents of param pbm to FILE pointer fp.
 */
void pbm_write(FILE* fp, pbm_t* pbm)
{
    char header[CHAR_BUF];
    memset(header, '\0', sizeof(header));

    sprintf(header, "P4\n%d %d\n", pbm->width, pbm->height);
    fputs(header, fp);

    int k = 0;
    int bitArr[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < pbm->height; ++i) {
        for (int j = 0; j < pbm->width + pbm->pad; j += 8) {
            bitArr[0] = pbm->buf[k++];
            bitArr[1] = pbm->buf[k++];
            bitArr[2] = pbm->buf[k++];
            bitArr[3] = pbm->buf[k++];
            bitArr[4] = pbm->buf[k++];
            bitArr[5] = pbm->buf[k++];
            bitArr[6] = pbm->buf[k++];
            bitArr[7] = pbm->buf[k++];

            uint8_t byte = packBits(bitArr);
            fwrite(&byte, 1, 1, fp);
        }
    }
}

/*
 * Extract individual bits from byte `c' and store in `pbm->buf' starting
 * at position `pos'. Fill pbm->buf with zeroed bits if col == pbm->width
 */
void extractBits(uint8_t c, int* pos, int* col, pbm_t* pbm)
{
    pbm->buf[(*pos)++] = c & 0x80 ? 1 : 0;
    if (*col == pbm->width) goto padRow;

    pbm->buf[(*pos)++] = c & 0x40 ? 1 : 0;
    if (*col == pbm->width) goto padRow;

    pbm->buf[(*pos)++] = c & 0x20 ? 1 : 0;
    if (*col == pbm->width) goto padRow;

    pbm->buf[(*pos)++] = c & 0x10 ? 1 : 0;
    if (*col == pbm->width) goto padRow;

    pbm->buf[(*pos)++] = c & 0x08 ? 1 : 0;
    if (*col == pbm->width) goto padRow;

    pbm->buf[(*pos)++] = c & 0x04 ? 1 : 0;
    if (*col == pbm->width) goto padRow;

    pbm->buf[(*pos)++] = c & 0x02 ? 1 : 0;
    if (*col == pbm->width) goto padRow;

    pbm->buf[(*pos)++] = c & 0x01 ? 1 : 0;
    if (*col == pbm->width) goto padRow;

    return;

padRow:

    *col = 0;
    for (int i = 0; i < pbm->pad; ++i) {
        pbm->buf[(*pos)++] = 0;
    }
}

uint8_t packBits(int bitArr[8])
{
    uint8_t c = 0x0;

    c |= bitArr[0] == 1 ? 0x80 : 0x0;
    c |= bitArr[1] == 1 ? 0x40 : 0x0;
    c |= bitArr[2] == 1 ? 0x20 : 0x0;
    c |= bitArr[3] == 1 ? 0x10 : 0x0;
    c |= bitArr[4] == 1 ? 0x08 : 0x0;
    c |= bitArr[5] == 1 ? 0x04 : 0x0;
    c |= bitArr[6] == 1 ? 0x02 : 0x0;
    c |= bitArr[7] == 1 ? 0x01 : 0x0;

    return c;
}

void pbm_print(pbm_t* pbm)
{
    int k = 0;
    for (int i = 0; i < pbm->height; ++i) {
        for (int j = 0; j < pbm->width + pbm->pad; ++j) {
            if (j % 8 == 0 && j > 0) printf(" ");
            printf("%d", pbm->buf[k++]);
        }
        printf("\n");
    }
}

void pbm_writeBlock(int block[4], int i, int j, pbm_t* pbm)
{
    pbm->buf[i * (pbm->width + pbm->pad) + j] = block[0];
    pbm->buf[i * (pbm->width + pbm->pad) + j + 1] = block[1];
    pbm->buf[(i + 1) * (pbm->width + pbm->pad) + j] = block[2];
    pbm->buf[(i + 1) * (pbm->width + pbm->pad) + j + 1] = block[3];
}

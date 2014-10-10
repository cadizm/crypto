
#include <stdlib.h>
#include <string.h>

#include "encrypt.h"
#include "stream.h"
#include "pbm.h"

/**
 * Globals
 */

static pbm_t* m_pbm = NULL;
static char* m_key = NULL;
static int m_pixelpos = 0;
static int m_curcol = 0;
static int m_keylen = 0;
static int m_keypos = 0;
static int m_keybits[8];
static int m_curbit = 0;

/**
 * Local functions
 */

static pbm_t* allocShare();
static void loadNextKeyByte();
static int getNextKeyBit();
static void resetPixelPos();
static int getNextPixelBit();

/**
 * Encrypt input from FILE pointer fp using passphrase pphrase.
 * Output is written to files using filenames derived from param
 * outfile. For example, if outfile is foo, files foo.1.pbm and
 * foo.2.pbm are used for output.
 */
void ss_encrypt(const char* pphrase, const char* outfile, FILE* fp)
{
    m_pbm = pbm_read(fp);
    m_keylen = m_pbm->size / 8;  /* m_pbm-size in bits, need bytes */
    m_key = stream(pphrase, m_keylen, 0);

    pbm_t* share1 = allocShare();
    pbm_t* share2 = allocShare();

    int block1001[] = {1, 0, 0, 1};
    int block0110[] = {0, 1, 1, 0};

    int i = 0;
    int j = 0;

    resetPixelPos();

    for (i = 0; i < share1->height; i += 2) {
        for (j = 0; j < share1->width; j += 2) {
            int pixel = getNextPixelBit();
            int key = getNextKeyBit();

            if (pixel == 0) {
                if (key == 0) {
                    pbm_writeBlock(block1001, i, j, share1);
                    pbm_writeBlock(block1001, i, j, share2);
                }
                else {
                    pbm_writeBlock(block0110, i, j, share1);
                    pbm_writeBlock(block0110, i, j, share2);
                }
            }
            else {
                if (key == 0) {
                    pbm_writeBlock(block1001, i, j, share1);
                    pbm_writeBlock(block0110, i, j, share2);
                }
                else {
                    pbm_writeBlock(block0110, i, j, share1);
                    pbm_writeBlock(block1001, i, j, share2);
                }
            }
        }
        for (; j < share1->width + share1->pad; ++j) {
        }
    }

    char outfile1[CHAR_BUF];
    char outfile2[CHAR_BUF];

    memset(outfile1, '\0', sizeof(outfile1));
    memset(outfile2, '\0', sizeof(outfile2));

    sprintf(outfile1, "%s.1.pbm", outfile);
    sprintf(outfile2, "%s.2.pbm", outfile);

    FILE* fp1 = NULL;
    FILE* fp2 = NULL;

    if (!(fp1 = fopen(outfile1, "wb"))) {
        char errorMsg[CHAR_BUF];
        sprintf(errorMsg, "Error opening %s for write", outfile1);
        perror(errorMsg);
        exit(-1);
    }

    if (!(fp2 = fopen(outfile2, "wb"))) {
        char errorMsg[CHAR_BUF];
        sprintf(errorMsg, "Error opening %s for write", outfile2);
        perror(errorMsg);
        exit(-1);
    }

    pbm_write(fp1, share1);
    pbm_write(fp2, share2);

    if (fp1) {
        fclose(fp1);
    }

    if (fp2) {
        fclose(fp2);
    }
}

pbm_t* allocShare()
{
    pbm_t* share = (pbm_t*)malloc(sizeof(pbm_t));

    if (!share) {
        char errorMsg[CHAR_BUF];
        sprintf(errorMsg, "Error allocating share.");
        perror(errorMsg);
        exit(-1);
    }

    share->width = m_pbm->width * 2;
    share->height = m_pbm->height * 2;

    if (share->width % 8 == 0) {
        share->pad = 0;
        share->size = share->height * share->width / 8;
    }
    else {
        int pad = 0;
        for (int width = share->width; width % 8 != 0; width++) {
            pad++;
        }
        share->pad = pad;
        share->size = share->height * (share->width / 8 + 1);
    }

    share->buf = malloc(share->height * (share->width + share->pad));
    memset(share->buf, 0, share->height * (share->width + share->pad));

    return share;
}

/**
 * Note: do not call this function directly. It is only meant to be
 * called internally from getNextKeyBit().
 */
void loadNextKeyByte()
{
    if (!m_key) {
        fprintf(stderr, "Error: m_key NULL\n");
        exit(-1);
    }

    uint8_t c = m_key[m_keypos++];

    m_keybits[0] = c & 0x80 ? 1 : 0;
    m_keybits[1] = c & 0x40 ? 1 : 0;
    m_keybits[2] = c & 0x20 ? 1 : 0;
    m_keybits[3] = c & 0x10 ? 1 : 0;
    m_keybits[4] = c & 0x08 ? 1 : 0;
    m_keybits[5] = c & 0x04 ? 1 : 0;
    m_keybits[6] = c & 0x02 ? 1 : 0;
    m_keybits[7] = c & 0x01 ? 1 : 0;
}

int getNextKeyBit()
{
    if (m_keypos == 0) {
        loadNextKeyByte();
    }

    if (m_curbit == 8) {
        m_curbit = 0;
        loadNextKeyByte();
    }

    int bitval = m_keybits[m_curbit++];

    return bitval;
}

void resetPixelPos()
{
    m_pixelpos = 0;
}

int getNextPixelBit()
{
    if (m_curcol == m_pbm->width) {
        m_curcol = 0;
        m_pixelpos += m_pbm->pad;
    }

    m_curcol++;

    return m_pbm->buf[m_pixelpos++];
}

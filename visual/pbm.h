
#ifndef PBM_H_
#define PBM_H_

#include <stdint.h>


/**
 * In memory representation of a pbm file.
 *
 * size  : size of buf in bits, not size of entire struct
 * pad   : used to fill bits to pad on byte boundaries
 * width : width of image in pixels/bits
 * height: height of image in pixels/bits
 * buf   : actual image data in single dimension
 */
typedef struct {
    int size;
    int pad;
    int width;
    int height;
    uint8_t* buf;
} pbm_t;


/**
 * For the following function that take a FILE pointer fp as an argument,
 * fp should be valid and already open if applicable.
 */

pbm_t* pbm_read(FILE* fp);
void pbm_write(FILE* fp, pbm_t* pbm);
void pbm_print(pbm_t* pbm);
void pbm_writeBlock(int block[4], int i, int j, pbm_t* pbm);

#endif /* PBM_H_ */

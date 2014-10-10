
#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>
#include <stdio.h>
#include <openssl/bn.h>

#include "defines.h"


void rndOddNum(int k, FILE* fp, BIGNUM* bn_res);
uint8_t rndByte(FILE* fp);

#endif /* UTIL_H_ */


#ifndef MILLER_RABIN_H_
#define MILLER_RABIN_H_

#include <stdint.h>
#include <stdio.h>
#include <openssl/bn.h>


/*
 * Returns:  0, if `bn_num' is a probable prime
 *          -1, if composite
 *          -2, either `maxitr' too large or not enough primes in `fp'
 */
int millerrabin(BIGNUM* bn_num, uint64_t maxitr, FILE* fp, int nIndent);

#endif /* MILLER_RABIN_H_ */

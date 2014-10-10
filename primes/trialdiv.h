
#ifndef TRIAL_DIV_H_
#define TRIAL_DIV_H_

#include <stdio.h>
#include <openssl/bn.h>


/*
 * Returns:  0, if `bn_num' is prime (trialdiv passes)
 *           m, the divisor, if found (trialdiv fails)
 *          -2, not enough primes in `fp' (trialdiv passes)
 */
int trialdiv(BIGNUM* bn_num, FILE* fp, int flagPrint);

#endif /* TRIAL_DIV_H_ */

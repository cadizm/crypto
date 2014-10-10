
#ifndef RND_SEARCH_H_
#define RND_SEARCH_H_

#include <stdio.h>
#include <openssl/bn.h>


BIGNUM* rndsearch(int k, int t, FILE* fpPrimes, FILE* fpRnd);

#endif /* RND_SEARCH_H_ */

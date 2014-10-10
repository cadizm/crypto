
#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>


void hexstring(int* buf, int n, uint8_t* res);

void unpack(uint8_t buf[8], int res[64]);
void pack(int buf[64], uint8_t res[8]);

void reverse(int buf[16][48], int n);

#endif /* UTIL_H_ */

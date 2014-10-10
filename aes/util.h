
#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

#include "defines.h"


void AddRoundKey(uint8_t state[4*Nb], uint32_t* w);

/*
 * hexstring should be of length 2 and in 00 to ff inclusive.
 */
int hex2dec(const char* hexstring);

uint8_t xtime(uint8_t b);
uint8_t gfdot(uint8_t a, uint8_t b);
void circlex(uint8_t a[4], uint8_t b[4], uint8_t res[4]);

/*
 * hexstring should be of length 2 and in 00 to ff inclusive.
 */
uint8_t gfinverse(uint8_t hexstring);

void printRcon();
void reverse(uint8_t buf[4]);

#endif /* UTIL_H_ */


#ifndef KEY_EXPAND_H_
#define KEY_EXPAND_H_

#include <stdint.h>

#include "defines.h"


void keyexpand(const char* k, const char* t, uint32_t w[Nb*(Nr+1)], int bPrint);
void KeyExpansion(uint8_t key[4*Nk], uint32_t w[Nb*(Nr+1)]);

#endif /* KEY_EXPAND_H_ */

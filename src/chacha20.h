#if !defined(__CHACHA20_H)
#define __CHACHA20_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/* Default block counter value */
#define CHACHA20_BLOCK 1

void chacha20_quarter_round(uint32_t x[16], uint32_t a, uint32_t b, uint32_t c, uint32_t d);
void chacha20_setup(uint32_t s[16], const unsigned char k[32], const unsigned char n[12], uint32_t c);
void chacha20_block(uint32_t s[16]);
void chacha20(const unsigned char k[32], const unsigned char n[12], unsigned char p[], uint32_t s, uint32_t c);
int chacha20_random(uint8_t *out, size_t bytes);

#endif

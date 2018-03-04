#if !defined(__MACRO_H)
#define __MACRO_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "crc32.h"

#define BYTE(a) ((a) & 0xFF)
#define WORD(a,b) ((BYTE(a) << 8) | BYTE(b))
#define DWORD(a,b,c,d) ((BYTE(a) << 24) | (BYTE(b) << 16) | (BYTE(c) << 8) | BYTE(d))

inline int min(int a, int b) {
    if (a > b)
        return b;
    return a;
}

inline int max(int a, int b) {
    if (a < b)
        return b;
    return a;
}

uint8_t bswap8(uint8_t x);
uint16_t bswap16(uint16_t x);
uint32_t bswap32(uint32_t x);
uint64_t bswap64(uint64_t x);

#endif /* __MACRO_H */

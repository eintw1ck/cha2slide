#include "macro.h"

inline uint8_t bswap8(uint8_t x)
{
	return x;
}

inline uint16_t bswap16(uint16_t x)
{
	return (((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8));
}

inline uint32_t bswap32(uint32_t x)
{
	return (((uint32_t)(bswap16((uint16_t)(x))) << 16) | (uint32_t)(bswap16((uint16_t)(x >> 16))));
}


inline uint64_t bswap64(uint64_t x)
{
	return (((uint64_t)(bswap32((uint32_t)(x))) << 32) | (uint64_t)(bswap32((uint32_t)(x >> 32))));
}
#if !defined(__MACRO_H)
#define __MACRO_H

#include <stdint.h>

#define BYTE(a) ((a) & 0xFF)
#define WORD(a,b) ((BYTE(a) << 8) | BYTE(b))
#define DWORD(a,b,c,d) ((BYTE(a) << 24) | (BYTE(b) << 16) | (BYTE(c) << 8) | BYTE(d))

extern inline uint8_t bswap8(uint8_t x);
extern inline uint16_t bswap16(uint16_t x);
extern inline uint32_t bswap32(uint32_t x);
extern inline uint64_t bswap64(uint64_t x);

#define WRITE_CHUNK(data, size) fwrite(data, size, 1, file)
#define WRITE_CHUNK_CRC(data, size) crc = crc32(data, size, crc); WRITE_CHUNK(data, size)
#define WRITE_CHUNK_BSWAP_NOCRC(data, bits) do { uint##bits##_t a = *(data); a = bswap##bits(a); WRITE_CHUNK(&a, bits/8); } while(0)
#define WRITE_CHUNK_BSWAP(data, bits) do { uint##bits##_t a = *(data); a = bswap##bits(a); WRITE_CHUNK_CRC(&a, bits/8); } while(0)

#define BEGIN_CHUNK(file, type) \
	int begin = ftell(file); \
	uint32_t crc = DEFAULT_CRC32; \
	fseek(file, 4, SEEK_CUR)

#define END_CHUNK(size) \
	int end = ftell(file); \
	fseek(file, begin, SEEK_SET); \
	WRITE_CHUNK_BSWAP_NOCRC(&size, 32); \
	fseek(file, end, SEEK_SET); \
	WRITE_CHUNK_BSWAP_NOCRC(&crc, 32)

#endif /* __MACRO_H */

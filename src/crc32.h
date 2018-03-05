#if !defined(__CRC32_H)
#define __CRC32_H

#include <stddef.h>
#include <stdint.h>

#define DEFAULT_CRC32 0

void crc32_gen_lut();

uint32_t crc32_(uint32_t crc, const void *data, size_t size);

#endif /* __CRC32_H */

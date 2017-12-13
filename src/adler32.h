#if !defined(__ADLER32_H)
#define __ADLER32_H

#include <stddef.h>
#include <stdint.h>

#define DEFAULT_ADLER32 1

uint32_t adler32(void *data, size_t size, uint32_t adler);

#endif

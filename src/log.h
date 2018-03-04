#if !defined(__LOG_H)
#define __LOG_H

#include "chunk.h"

enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
};

void pprintf(int level, const char* format, ...);
void print_chunk(png_chunk *chunk);

#endif /* __LOG_H */

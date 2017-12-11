#include "adler32.h"

/**
 * @brief      Computes adler32 checksum
 *
 * @param      data   The input data to compute checksum for
 * @param[in]  size   The data size
 * @param[in]  adler  The current adler checksum if continuing from a prior chunk.
 *
 * @return     The adler32 checksum.
 */

uint32_t adler32(void *data, size_t size, uint32_t adler)
{
    uint32_t s1 = adler & 0xFFFF;
    uint32_t s2 = (adler >> 16);
    for (size_t i = 0; i < size; i++) {
        s1 = (s1 + ((char*)data)[i]) % 65521;
        s2 = (s2 + s1) % 65521;
    }
    return (s2 << 16) + s1;
}
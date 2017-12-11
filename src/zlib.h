#if !defined(__ZLIB_H)
#define __ZLIB_H

#include <stdint.h>
#include <math.h>
#include "adler32.h"

/* Compression Method and Compression Flags
 * 
 * This is the first byte in the zlib header.
 * 
 * The first nibble corresponds to information about the encoder properties.
 * The second nibble represents the window size such that `window_size = pow2(nibble+8);`.
 * (Nibbles greater than 7 are not allowed). */
enum zlib_cmf
{
    zlib_cmf_method_deflate = 1 << 3,
};

/* returns cinfo of a window size in the upper 4 bits*/
static inline uint8_t zlib_cinfo(uint32_t window_size) {
    return ((uint8_t)ceil(log2((double)(window_size ? window_size : 32768))) - 8) << 4;
}

/* Flags
 * 
 * This is the second byte in the zlib header.
 * 
 * It contains the fcheck value in bits 0-4 and the fdict and flevel in bits 5-8.
 */
enum zlib_flg
{
    zlib_fdict = 1 << 5, /* compressor used a preset dictionary */
    zlib_flevel_fastest = 0 << 6, /* compressor used fastest algorithm */
    zlib_flevel_fast = 1 << 6, /* compressor used fast algorithm */
    zlib_flevel_default = 2 << 6, /* compressor used default algorithm */
    zlib_flevel_maximum = 3 << 6, /* compressor used maximum compression, slowest algorithm */
};

/**
 * @brief      Fcheck function
 *
 * @param[in]  cmf_flg  The (cmf*256 + flg) value.
 *
 * @return     Returns the input if the check succeeded, otherwise, returns cmf_flg + cmf_flg % 31.
 */
static inline uint16_t zlib_fcheck(uint16_t cmf_flg)
{
//    return cmf_flg + (!!(cmf_flg % 31) * (31 - (cmf_flg % 31)));
    return cmf_flg + (-(!!(cmf_flg % 31)) & (31 - (cmf_flg % 31)));
}

#endif /* __ZLIB_H */

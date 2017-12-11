#include "crc32.h"
#include "macro.h"

#include "munit.h"

/**
 * Static look-up table enabling byte-wise computation of CRC32 values.
 */
static uint32_t crc32_lut[256] = {0};

/**
 * @brief      Generate CRC32 using reverse polynomial 0xEDB88320
 */
void crc32_gen_lut()
{
    for (uint16_t i = 0; i <= 0xFF; i++) {
          uint32_t crc = i;
          for (uint8_t j = 0; j < 8; j++)
              /* If crc is even, XOR polynomial.
               * 
               * This code is functionally equivalent to:
               * if (crc & 1)
               *     crc = (crc >> 1);
               * else
               *     crc = (crc >> 1) ^ 0xEDB88320;
               */
               crc = (crc >> 1) ^ (-(crc & 1) & 0xEDB88320);
          crc32_lut[i] = crc;
    }
}

/**
 * @brief      Calculate a CRC32 value.
 *
 * @param[in]  data  The data of which CRC to calculate.
 * @param[in]  size  The size of data.
 * @param[in]  crc   The input CRC32 value, this should be zero if not continuing to check previous data.
 *
 * @return     CRC32 value
 */
uint32_t crc32(const void* data, size_t size, uint32_t crc)
{
    /* Generate table if it has not already been generated. */
    if (!crc32_lut[1]) crc32_gen_lut();
    crc = ~crc;
    while (size--) crc = (crc >> 8) ^ crc32_lut[(BYTE(crc)) ^ *(uint8_t*)data++];
    return ~crc;
}

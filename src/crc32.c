#include <pthread.h>

#include "crc32.h"

/**
 * Static look-up table enabling byte-wise computation of CRC32 values.
 * 256 values for representing all the single byte values 0x00-0xFF.
 */
static uint32_t crc32_lut[256] = {0};
/**
 * Any functions writing to the look-up table should use the mutex to
 * ensure that no multiple writes occur across threads. There is also
 * pthread_once_t to make sure that the generation code is run at least
 * once before the table is used.
 */
static pthread_mutex_t crc32_lut_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t crc32_lut_gen = PTHREAD_ONCE_INIT;

/**
 * @brief      Generate CRC32 using reverse polynomial
 *
 * $x^{32}+x^{26}+x^{23}+x^{22}+x^{16}+x^{12}+x^{11}+x^{10}+x^8+x^7+x^5+x^4+x^2+x+1$
 *
 * 1 is defined as the MSB, so if you set x equal to 1 then you get the following
 * binary `1110 1101 1011 1000 1000 0011 0010 0000' which is `0xEDB88320'.
 */
void crc32_gen_lut()
{
    /* Initialise crc32_lut mutex with no attributes */
    pthread_mutex_init(&crc32_lut_mutex, NULL);
    /* Lock thread for writing look-up table. */
    pthread_mutex_lock(&crc32_lut_mutex);
    for (uint16_t i = 0; i <= 0xFF; i++) {
          uint32_t crc = i;
          for (uint8_t j = 0; j < 8; j++)
              /* If crc is even, XOR polynomial. The reverse polynomial
               * indicates that we are doing right shifts.
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
    /* Unlock mutex to release ownership of look-up table */
    pthread_mutex_unlock(&crc32_lut_mutex);
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
uint32_t crc32_(uint32_t crc, const void* data, size_t size)
{
    /**
     * Generate table if it has not already been generated. You could also
     * check if crc32_lut[1] is equal to the reverse polynomial we are using,
     * but this wouldn't be thread-safe.
     */
    pthread_once(&crc32_lut_gen, crc32_gen_lut);
    crc = ~crc;
    while (size--) crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ *(uint8_t*)data++];
    return ~crc;
}

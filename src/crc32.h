#if !defined(__CRC32_H)
#define __CRC32_H

#include <stdint.h>
#include <stddef.h>

#define DEFAULT_CRC32 0

/**
 * @brief      Generate CRC32 using reverse polynomial 0xEDB88320
 */
void crc32_gen_lut();

/**
 * @brief      Calculate a CRC32 value.
 *
 * @param[in]  crc   The input CRC32 value, this should be zero if not continuing to check previous data.
 * @param[in]  data  The data of which CRC to calculate.
 * @param[in]  size  The size of data.
 *
 * @return     CRC32 value
 */
uint32_t crc32_(uint32_t crc, const void *data, size_t size);

#endif /* __CRC32_H */

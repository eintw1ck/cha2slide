#if !defined(__BLAKE2B_H)
#define __BLAKE2B_H

#include <stdint.h>

typedef struct {
   uint64_t state[8];
   uint64_t hashed[2];
   uint8_t buffer[128];
   size_t blen;
   size_t hlen;
} blake2b_ctx;

int blake2b_setup(blake2b_ctx *ctx, size_t hlen, uint8_t *key, size_t klen);
void blake2b_update(blake2b_ctx *ctx, uint8_t *message, size_t mlen);
void blake2b_final(blake2b_ctx *ctx, uint8_t *hash);
void blake2b(uint8_t *message, size_t mlen, uint8_t *key, size_t klen, uint8_t *hash, size_t hashlen);

#endif /* __BLAKE2B_H */

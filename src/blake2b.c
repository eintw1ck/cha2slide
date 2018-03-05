#include "blake2b.h"

static inline uint64_t rotr64(uint64_t x, uint8_t n)
{
    return ((x >> n) | (x << (64 - n)));
}

static const uint64_t blake2b_iv[8] = {
    0x6A09E667F3BCC908,
    0xBB67AE8584CAA73B,
    0x3C6EF372FE94F82B,
    0xA54FF53A5F1D36F1,
    0x510E527FADE682D1,
    0x9B05688C2B3E6C1F,
    0x1F83D9ABFB41BD6B,
    0x5BE0CD19137E2179,
};

static uint8_t blake2b_sigma[10][16] = {
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15 },
    { 14, 10, 4,  8,  9,  15, 13, 6,  1,  12, 0,  2,  11, 7,  5,  3  },
    { 11, 8,  12, 0,  5,  2,  15, 13, 10, 14, 3,  6,  7,  1,  9,  4  },
    { 7,  9,  3,  1,  13, 12, 11, 14, 2,  6,  5,  10, 4,  0,  15, 8  },
    { 9,  0,  5,  7,  2,  4,  10, 15, 14, 1,  11, 12, 6,  8,  3,  13 },
    { 2,  12, 6,  10, 0,  11, 8,  3,  4,  13, 7,  5,  15, 14, 1,  9  },
    { 12,  5, 1,  15, 14, 13, 4,  10, 0,  7,  6,  3,  9,  2,  8,  11 },
    { 13, 11, 7,  14, 12, 1,  3,  9,  5,  0,  15, 4,  8,  6,  2,  10 },
    { 6,  15, 14, 9,  11, 3,  0,  8,  12, 2,  13, 7,  1,  4,  10, 5  },
    { 10,  2, 8,  4,  7,  6,  1,  5,  15, 11, 9,  14, 3,  12, 13, 0  },
};

static inline void blake2b_round(uint64_t t[16], uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint64_t x, uint64_t y)
{
    t[a] = t[a] + t[b] + x;
    t[d] = rotr64(t[d] ^ t[a], 32);

    t[c] = t[c] + t[d];
    t[b] = rotr64(t[b] ^ t[c], 24);

    t[a] = t[a] + t[b] + y;
    t[d] = rotr64(t[d] ^ t[a], 16);

    t[c] = t[c] + t[d];
    t[b] = rotr64(t[b] ^ t[c], 63);
}

static void blake2b_block(uint64_t state[8], uint8_t buffer[128], uint64_t hashed[2], uint8_t last)
{
    uint64_t work[16];

    for (int i = 0; i < 8; i++) {
        work[i] = state[i];
        work[i + 8] = blake2b_iv[i];
    }

    work[12] ^= hashed[0];
    work[13] ^= hashed[1];
    if (last) work[14] = ~work[14];

    for (int i = 0; i < 12; i++) {
#define mix(a, b, c, d, x) blake2b_round(work, a, b, c, d, ((uint64_t*)buffer)[blake2b_sigma[i%10][x]], ((uint64_t*)buffer)[blake2b_sigma[i%10][x+1]])
        mix(0, 4, 8,  12, 0);
        mix(1, 5, 9,  13, 2);
        mix(2, 6, 10, 14, 4);
        mix(3, 7, 11, 15, 6);

        mix(0, 5, 10, 15, 8);
        mix(1, 6, 11, 12, 10);
        mix(2, 7, 8,  13, 12);
        mix(3, 4, 9,  14, 14);
#undef mix
   }

   for(int i = 0; i < 8; i++)
       state[i] ^= work[i] ^ work[i+8];
}

int blake2b_setup(blake2b_ctx *ctx, size_t hlen, uint8_t *key, size_t klen)
{
    if (hlen == 0 || hlen > 64 || klen > 64)
        return -1;

    ctx->state[0] = blake2b_iv[0] ^ 0x01010000 ^ (klen << 8) ^ hlen;
    for (size_t i = 1; i < 8; i++) ctx->state[i] = blake2b_iv[i];

    ctx->hashed[0] = 0;
    ctx->hashed[1] = 0;
    ctx->blen = 0;
    ctx->hlen = hlen;

    for (size_t i = klen; i < 128; i++)
        ctx->buffer[i] = 0;
    if (klen > 0) {
        blake2b_update(ctx, key, klen);
        ctx->blen = 128;
    }

    return 0;
}

void blake2b_update(blake2b_ctx *ctx, uint8_t *message, size_t mlen)
{
    for (size_t i = 0; i < mlen; i++) {
        if (ctx->blen == 128) {
            ctx->hashed[0] += ctx->blen;
            if (ctx->hashed[0] < ctx->blen) ctx->hashed[1]++;
            blake2b_block(ctx->state, ctx->buffer, ctx->hashed, 0);
            ctx->blen = 0;
        }
        ctx->buffer[ctx->blen++] = message[i];
    }
}

void blake2b_final(blake2b_ctx *ctx, uint8_t *hash)
{

    ctx->hashed[0] += ctx->blen;
    /* increment high 64bit counter via integer overflow */
    if (ctx->hashed[0] < ctx->blen)
        ctx->hashed[1]++;

    while (ctx->blen < 128)
        ctx->buffer[ctx->blen++] = 0;
    blake2b_block(ctx->state, ctx->buffer, ctx->hashed, 1);

    for (size_t i = 0; i < ctx->hlen; i++)
        hash[i] = ((uint8_t*)ctx->state)[i];
}

void blake2b(uint8_t *in, size_t mlen, uint8_t *key, size_t klen, uint8_t *hash, size_t hlen)
{
    blake2b_ctx ctx;

    if (blake2b_setup(&ctx, hlen, key, klen))
        return;
    blake2b_update(&ctx, in, mlen);
    blake2b_final(&ctx, hash);

    return;
}
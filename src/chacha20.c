#include "chacha20.h"

static inline uint32_t rotl(uint32_t x, uint32_t n)
{
    return ((x << n) | (x >> (-n & 31)));
}

void chacha20_quarter_round(uint32_t x[16], uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    x[a] += x[b];
    x[d] = rotl(x[d] ^ x[a], 16);
    x[c] += x[d];
    x[b] = rotl(x[b] ^ x[c], 12);
    x[a] += x[b];
    x[d] = rotl(x[d] ^ x[a], 8);
    x[c] += x[d];
    x[b] = rotl(x[b] ^ x[c], 7);
}

void chacha20_setup(uint32_t s[16], const uint8_t k[32], const uint8_t n[12], uint32_t c)
{
    s[0] = 0x61707865;
    s[1] = 0x3320646e;
    s[2] = 0x79622d32;
    s[3] = 0x6b206574;
#define load(x, i) x[i + 3] << 24 | x[i + 2] << 16 | x[i + 1] << 8 | x[i]
    for (int i = 0; i < 32; i += 4)
        s[i / 4 + 4] = load(k, i);
    s[12] = c;
    for (int i = 0; i < 12; i += 4)
        s[i / 4 + 13] = load(n, i);
#undef load
}

void chacha20_block(uint32_t s[16])
{
    uint32_t state[16] = {0};

    for (int i = 0; i < 16; i++)
        state[i] = s[i];

    for (int i = 0; i < 10; i++) {
        chacha20_quarter_round(state, 0, 4, 8, 12);
        chacha20_quarter_round(state, 1, 5, 9, 13);
        chacha20_quarter_round(state, 2, 6, 10, 14);
        chacha20_quarter_round(state, 3, 7, 11, 15);
        chacha20_quarter_round(state, 0, 5, 10, 15);
        chacha20_quarter_round(state, 1, 6, 11, 12);
        chacha20_quarter_round(state, 2, 7, 8, 13);
        chacha20_quarter_round(state, 3, 4, 9, 14);
    }
    for (int i = 0; i < 16; i++)
        s[i] += state[i];
}

/* k - key, n - nonce, p - plaintext, s - size, c - block count */
void chacha20(const uint8_t k[32], const uint8_t n[12], uint8_t p[], uint32_t s, uint32_t block)
{
    uint32_t t[16] = {0};
    for (uint32_t c = 0;; c++) {
        chacha20_setup(t, k, n, c + block);
        chacha20_block(t);
        for (uint32_t j = 0; j < ((s <= 64) ? s : 64); j++)
            /* blocks start at 1 for some reason */
            p[c * 64 + j] ^= ((uint8_t *)t)[j];
        if (s <= 64)
            return;
        s -= 64;
    }
}

int chacha20_random(uint8_t *out, size_t bytes)
{
    uint8_t k[32] = {0};
    uint8_t n[12] = {0};
    uint32_t state[16] = {0};

    if (!out && !bytes)
        return 1;

    /* Period of chacha20 with a 32bit block counter only 256 GiB, could probably call recursively
     * for a near infinite period but just error out instead.
     */
    if (bytes > 0xFFFFFFFF * 0x40)
        return 2;

    srand(time(NULL));

    for (int i = 0; i < 32; i++)
        k[i] = rand();
    for (int i = 0; i < 12; i++)
        n[i] = rand();

    chacha20_setup(state, k, n, CHACHA20_BLOCK);

    while (bytes > 0) {
        int len = (bytes > 64) ? 64 : bytes;
        chacha20_block(state);
        memcpy(out, state, len);
        bytes -= len;
    }

    return 0;
}
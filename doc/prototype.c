#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h>

static inline uint32_t rotl(uint32_t x, uint32_t n)
{
    return ((x<<n) | (x>>(-n&31)));
}

static inline void quarter_round(uint32_t x[16], uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    x[a] += x[b]; x[d] = rotl(x[d] ^ x[a], 16);
    x[c] += x[d]; x[b] = rotl(x[b] ^ x[c], 12);
    x[a] += x[b]; x[d] = rotl(x[d] ^ x[a], 8);
    x[c] += x[d]; x[b] = rotl(x[b] ^ x[c], 7);
}

void setup(uint32_t s[16], const unsigned char k[32], const unsigned char n[12], uint32_t c)
{
    s[0] = 0x61707865;
    s[1] = 0x3320646e;
    s[2] = 0x79622d32;
    s[3] = 0x6b206574;
#define load(x, i) x[i+3] << 24 | x[i+2] << 16 | x[i+1] << 8 | x[i]
    for (int i = 0; i < 32; i+=4)
        s[i/4+4] = load(k, i);
    s[12] = c;
    for (int i = 0; i < 12; i+=4)
        s[i/4+13] = load(n, i);
#undef load
}

void block(uint32_t s[16], const unsigned char k[32], const unsigned char n[12], uint32_t c)
{
    uint32_t state[16] = {0};
    memcpy(state, s, 64);
    for (int i = 0; i < 10; i++) {
        quarter_round(state, 0, 4, 8,  12);
        quarter_round(state, 1, 5, 9,  13);
        quarter_round(state, 2, 6, 10, 14);
        quarter_round(state, 3, 7, 11, 15);
        quarter_round(state, 0, 5, 10, 15);
        quarter_round(state, 1, 6, 11, 12);
        quarter_round(state, 2, 7, 8,  13);
        quarter_round(state, 3, 4, 9,  14);
    }
    for (int i = 0; i < 16; i++)
        s[i] += state[i];
}

/* k - key, n - nonce, p - plaintext, s - size, c - count */
void encrypt(const unsigned char k[32], const unsigned char n[12], unsigned char p[], uint32_t s, uint32_t c)
{
    uint32_t t[16] = {0};
    for (;; c++) {
        setup(t, k, n, c);
        block(t, k, n, c);
        for (int j = 0; j < ((s <= 64) ? s : 64); j++)
                      /* blocks start at 1 for some reason */
            p[((c < 1) ? 0 : c - 1)*64+j] ^= ((uint8_t*)t)[j];
        if (s <= 64)
            return;
        s -= 64;
    }
}

#define TESTS
#if defined(TESTS)

void hexdump(void *x, uint32_t length)
{
    for (uint32_t i = 0; i < length + (length % 16 ? 16 - length % 16 : 0); i++) {
        // initial byte
        if (i % 16 == 0)
            printf("0x%08x (%03x): ", (uint32_t)x+i, i);

        // allows for unbounded lengths on the outer for
        if (i < length)
            printf("%02x ", ((char*)x)[i]);
        else
            printf("   ");

        // last byte in row
        if (i % 16 == 15) {
            // ascii view
            for (uint32_t j = i - 15; j <= i; j++) {
                if(j >= length)
                    putchar(' ');
                else if (isprint((char*)x)[j])
                    putchar(((char*)x)[j]);
                else
                    putchar('.');
            }
            putchar('\n');
        }
    }
}

#define check_states(a, b) for (int i = 0; i < 16; i++) if (a[i] != b[i]) goto err_ ## b;

int test_quarter_round()
{
    uint32_t expected[16] = {0x879531e0, 0xc5ecf37d, 0xbdb886dc, 0xc9a62f8a, 0x44c20ef3, 0x3390af7f, 0xd9fc690b, 0xcfacafd2, 0xe46bea80, 0xb00a5631, 0x974c541a, 0x359e9963, 0x5c971061, 0xccc07c79, 0x2098d9d6, 0x91dbd320};
    uint32_t state[16] = {0x879531e0, 0xc5ecf37d, 0x516461b1, 0xc9a62f8a, 0x44c20ef3, 0x3390af7f, 0xd9fc690b, 0x2a5f714c, 0x53372767, 0xb00a5631, 0x974c541a, 0x359e9963, 0x5c971061, 0x3d631689, 0x2098d9d6, 0x91dbd320};
    quarter_round(state, 2,7,8,13);
    check_states(expected, state);
    printf("quarter_round: passed.\n");
    return 1;
err_state:
    printf("quarter_round: failed.\n");
    printf("state: \n");
    for (int i = 0; i < 16; i++)
        printf("0x%04x ", state[i]);
    printf("\n");
    printf("expected: \n");
    for (int i = 0; i < 16; i++)
        printf("0x%04x, ", expected[i]);
    printf("\n");
    return 0;
}
int test_setup()
{
    uint32_t expected[16] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574, 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c, 0x00000001, 0x09000000, 0x4a000000, 0x00000000};
    const unsigned char key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
    const unsigned char nonce[12] = {0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x00};
    uint32_t state[16] = {0};
    setup(state, key, nonce, 1);
    check_states(expected, state);
    printf("setup: passed.\n");
    return 1;
err_state:
    printf("setup: failed.\n");
    printf("state: \n");
    hexdump(state, 64);
    printf("expected: \n");
    hexdump(expected, 64);
    return 0;
}
int test_block()
{
    uint32_t expected[16] = {0xe4e7f110, 0x15593bd1, 0x1fdd0f50, 0xc47120a3, 0xc7f4d1c7, 0x0368c033, 0x9aaa2204, 0x4e6cd4c3, 0x466482d2, 0x09aa9f07, 0x05d7c214, 0xa2028bd9, 0xd19c12b5, 0xb94e16de, 0xe883d0cb, 0x4e3c50a2};
    const unsigned char key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
    const unsigned char nonce[12] = {0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x00};
    uint32_t state[16] = {0};
    setup(state, key, nonce, 1);
    block(state, key, nonce, 1);
    check_states(expected, state);
    printf("block: passed.\n");
    return 1;
err_state:
    printf("block: failed.\n");
    printf("state: \n");
    hexdump(state, 64);
    printf("expected: \n");
    hexdump(expected, 64);
    return 0;
}

int test_stream()
{
    const unsigned char key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
    const unsigned char nonce[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x00};
    uint32_t expected_setup_state_1[16] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574, 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c, 0x00000001, 0x00000000, 0x4a000000, 0x00000000};
    uint32_t expected_setup_state_2[16] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574, 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c, 0x00000002, 0x00000000, 0x4a000000, 0x00000000};
    uint32_t expected_state_1[16] = {0xf3514f22, 0xe1d91b40, 0x6f27de2f, 0xed1d63b8, 0x821f138c, 0xe2062c3d, 0xecca4f7e, 0x78cff39e, 0xa30a3b8a, 0x920a6072, 0xcd7479b5, 0x34932bed, 0x40ba4c79, 0xcd343ec6, 0x4c2c21ea, 0xb7417df0};
    uint32_t expected_state_2[16] = {0x9f74a669, 0x410f633f, 0x28feca22, 0x7ec44dec, 0x6d34d426, 0x738cb970, 0x3ac5e9f3, 0x45590cc4, 0xda6e8b39, 0x892c831a, 0xcdea67c1, 0x2b7e1d90, 0x037463f3, 0xa11a2073, 0xe8bcfb88, 0xedc49139};
    uint32_t setup_state_1[16] = {0};
    uint32_t setup_state_2[16] = {0};
    uint32_t state_1[16] = {0};
    uint32_t state_2[16] = {0};

    setup(setup_state_1, key, nonce, 1);
    check_states(expected_setup_state_1, setup_state_1);
    printf("setup_state_1: passed.\n");

    setup(setup_state_2, key, nonce, 2);
    check_states(expected_setup_state_2, setup_state_2);
    printf("setup_state_2: passed.\n");

    memcpy(state_1, setup_state_1, 64);
    block(state_1, key, nonce, 1);
    check_states(expected_state_1, state_1);
    printf("state_1: passed.\n");

    memcpy(state_2, setup_state_2, 64);
    block(state_2, key, nonce, 2);
    check_states(expected_state_2, state_2);
    printf("state_2: passed.\n");

    return 1;
err_setup_state_1:
    printf("setup_state_1: failed.\n");
    printf("state: \n");
    hexdump(setup_state_1, 64);
    printf("expected: \n");
    hexdump(expected_setup_state_1, 64);
    return 0;
err_setup_state_2:
    printf("setup_state_2: failed.\n");
    printf("state: \n");
    hexdump(setup_state_2, 64);
    printf("expected: \n");
    hexdump(expected_setup_state_2, 64);
    return 0;
err_state_1:
    printf("state_1: failed.\n");
    printf("state: \n");
    hexdump(state_1, 64);
    printf("expected: \n");
    hexdump(expected_state_1, 64);
    return 0;
err_state_2:
    printf("state_2: failed.\n");
    printf("state: \n");
    hexdump(state_2, 64);
    printf("expected: \n");
    hexdump(expected_state_2, 64);
    return 0;
}

int test_encrypt()
{
    unsigned char expected[] = {0x6e, 0x2e, 0x35, 0x9a, 0x25, 0x68, 0xf9, 0x80, 0x41, 0xba, 0x07, 0x28, 0xdd, 0x0d, 0x69, 0x81, 0xe9, 0x7e, 0x7a, 0xec, 0x1d, 0x43, 0x60, 0xc2, 0x0a, 0x27, 0xaf, 0xcc, 0xfd, 0x9f, 0xae, 0x0b, 0xf9, 0x1b, 0x65, 0xc5, 0x52, 0x47, 0x33, 0xab, 0x8f, 0x59, 0x3d, 0xab, 0xcd, 0x62, 0xb3, 0x57, 0x16, 0x39, 0xd6, 0x24, 0xe6, 0x51, 0x52, 0xab, 0x8f, 0x53, 0x0c, 0x35, 0x9f, 0x08, 0x61, 0xd8, 0x07, 0xca, 0x0d, 0xbf, 0x50, 0x0d, 0x6a, 0x61, 0x56, 0xa3, 0x8e, 0x08, 0x8a, 0x22, 0xb6, 0x5e, 0x52, 0xbc, 0x51, 0x4d, 0x16, 0xcc, 0xf8, 0x06, 0x81, 0x8c, 0xe9, 0x1a, 0xb7, 0x79, 0x37, 0x36, 0x5a, 0xf9, 0x0b, 0xbf, 0x74, 0xa3, 0x5b, 0xe6, 0xb4, 0x0b, 0x8e, 0xed, 0xf2, 0x78, 0x5e, 0x42, 0x87, 0x4d};
    const unsigned char key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
    const unsigned char nonce[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x00};
    unsigned char *string = (unsigned char*)strdup("Ladies and Gentlemen of the class of '99: If I could offer you only one tip for the future, sunscreen would be it.");
    uint32_t length = strlen((char*)string);
    encrypt(key, nonce, string, length, 1);
    for (int i = 0; i < length; i++) if (string[i] != expected[i]) goto err_string;
    printf("encrypt: passed.\n");
    return 0;
err_string:
    printf("encrypt: failed.\n");
    printf("string: \n");
    hexdump(string, length);
    printf("expected: \n");
    hexdump(expected, length);
    return 1;
}

#endif

int main() {
#if defined(TESTS)
    test_quarter_round();
    test_setup();
    test_block();
    test_stream();
    test_encrypt();
#endif
    return 0;
}

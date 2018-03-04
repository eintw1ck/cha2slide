#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #include <Windows.h>
#endif

/*

cha2slide encode ..\bob.225.rgb out.png 225
cha2slide encrypt out.png out.enc test
cha2slide decrypt out.enc alt.png test

*/

#include <errno.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chacha20.h"
#include "chunk.h"
#include "log.h"

#define CHA2SLIDE_VER_MAJOR 0
#define CHA2SLIDE_VER_MINOR 0
#define CHA2SLIDE_VER_PATCH 1

static void header()
{
    fprintf(stderr, "\x1B[4mChaChaSlide\x1B[0m \x1B[7mver. %d.%d.%d\x1B[0m\n\t\x1B[1mTake it back now y'all.\x1B[0m\n", CHA2SLIDE_VER_MAJOR, CHA2SLIDE_VER_MINOR, CHA2SLIDE_VER_PATCH);
}

static void footer()
{
    srand(time(NULL));

    char *bob[13] = {
        "We don't make mistakes, just happy little accidents.",
        "Talent is a pursued interest. Anything that you're willing to practice, you can do.",
        "Let's get crazy.",
        "The secret to doing anything is believing that you can do it. Anything that you believe you can do strong enough, you can do. Anything. As long as you believe.",
        "There's nothing wrong with having a tree as a friend.",
        "I can't think of anything more rewarding than being able to express yourself to others through painting. Exercising the imagination, experimenting with talents, being creative; these things, to me, are truly the windows to your soul.",
        "Mix up a little more shadow color here, then we can put us a little shadow right in there. See how you can move things around? You have unlimited power on this canvas -- can literally, literally move mountains",
        "Look around. Look at what we have. Beauty is everywhere -- you only have to look to see it.",
        "Just go out and talk to a tree. Make friends with it.",
        "Trees cover up a multitude of sins.",
        "You too can paint almighty pictures.",
        "Like I always say, 'Everyone needs a friend.'",
        "Water's like me. It's lazy. Boy, it always looks for the easiest way to do things.",
    };
    fprintf(stderr, "\x1b[32m%s\x1b[0m - Bob (1942-1995)\n", bob[rand() % 13]);
}

static void usage()
{
    fprintf(stderr,
        "Usage: cha2slide <command> <input-file> <output-file> <command-dependent-argument>\n"
        "Commands:\n"
        "\tencrypt\n"
        "\tdecrypt\n"
        "\t\tthese take a password argument\n"
        "\tencode\n"
        "\t\tthis takes a pixel width argument\n"
    );
}

uint32_t parseuint(char *str)
{
    errno = 0;
    uint32_t x = strtoul(str + strspn(str, "="), NULL, 0);
    if (errno) {
        pprintf(LOG_ERROR, "%s \n", strerror(errno));
        exit(errno);
    }
    return x;
}

int encode(FILE *input, FILE *output, uint32_t width)
{
    off_t size;
    uint8_t *pixel_data = NULL;

    fseeko(input, 0, SEEK_END);
    size = ftello(input);
    rewind(input);
    pixel_data = malloc(size);
    fread(pixel_data, size, 1, input);

    if ((uint32_t)size % width)
        pprintf(LOG_WARNING, "Input data length (%d) is not divisable by given width (%d). Truncating data, this may output an incorrect image.", size, width);
    else
        pprintf(LOG_INFO, "size %d", size);
    png_chunk **chunks = malloc(4*sizeof(png_chunk*));
    for (int i = 0; i < 4; i++) chunks[i] = NULL;

    int ret = 0;
    if ((ret = png_new_ihdr((png_ihdr**)&chunks[0], width, size/(width*3), 8, png_color_truecolor, png_interlace_none))) {
        pprintf(LOG_ERROR, "Couldn't create new IHDR chunk: %01x.", ret);
    }
    print_chunk(chunks[0]);
    png_new_idat((png_idat**)&chunks[1], pixel_data, size, width*3);
    print_chunk(chunks[1]);
    png_new_iend((png_iend**)&chunks[2]);

    png_write(output, chunks);
    
    free(pixel_data);

    return 0;
}

int crypto(FILE *input, FILE *output, int encrypt, char *password)
{
    off_t size;
    fseeko(input, 0, SEEK_END);
    size = ftello(input);
    rewind(input);

    uint8_t key[32];
    uint8_t salt[22];
    uint8_t nonce[10];

    char buffer[64];
    size_t bytes;

    size_t chacha_block = CHACHA20_BLOCK;

    if (encrypt) {
        chacha20_random(salt, 22);
        chacha20_random(nonce, 10);
        fwrite(salt, 1, 22, output);
        fwrite(nonce, 1, 10, output);
    } else {
        fread(salt, 1, 22, input);
        fread(nonce, 1, 10, input);
    }

    blake2b(password, strlen(password), salt, 22, key, 32);

    while (0 < (bytes = fread(buffer, 1, 64, input))) {
        chacha20(key, nonce, buffer, bytes, chacha_block++);
        fwrite(buffer, 1, bytes, output);
    }

    return 0;
}

int main(int argc, char *argv[argc])
{
    FILE *input = NULL, *output = NULL;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING 
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x4
#endif

    /* enable colours on windoze */

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(handle, &mode);
    mode |= (uint32_t)ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(handle, mode)) {
        printf("Couldn't set ANSI escape mode. Exiting...\n");
        exit(EXIT_FAILURE);
    }

#endif

    header();

    if (argc < 4) {
        usage();
        exit(1);
    }

    if (!strcmp(argv[2], argv[3])) {
        pprintf(LOG_ERROR, "Input and output cannot be the same file.");
        goto err;
    }

    if(access(argv[2], F_OK) != -1) {
        pprintf(LOG_INFO, "Input file \x1B[7m%s\x1B[0m", argv[2]);
        input = fopen(argv[2], "rb");
    } else {
        pprintf(LOG_ERROR, "Input file not found.");
        goto err;
    }

    if(access(argv[3], F_OK) != -1) {
        pprintf(LOG_WARNING, "Output file exists, overwriting.");
    }
    pprintf(LOG_INFO, "Output file \x1B[7m%s\x1B[0m", argv[3]);
    output = fopen(argv[3], "wb");

    if (!strcmp(argv[1], "encode")) {
        pprintf(LOG_INFO, "Mode \x1B[7mencode\x1B[0m");
        encode(input, output, parseuint(argv[4]));
    } else if (!strcmp(argv[1], "encrypt")) {
        pprintf(LOG_INFO, "Mode \x1B[7mencrypt\x1B[0m");
        crypto(input, output, 1, argv[4]);
    } else if (!strcmp(argv[1], "decrypt")) {
        pprintf(LOG_INFO, "Mode \x1B[7mdecrypt\x1B[0m");
        crypto(input, output, 0, argv[4]);
    }

err:
    if (input) {
        fclose(input);
    }
    if (output) {
        fclose(output);
    }

    footer();
    return 0;
}

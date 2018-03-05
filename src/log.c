#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <signal.h>

char *LOG_LEVEL[] = {
    "?", /* debug info */
    "#", /* info */
    "*", /* warning */
    "!", /* error */
};

char *LOG_LEVEL_COLOR[] = {
    "\x1B[36m", /* debug info */
    "\x1B[32m", /* info */
    "\x1B[33m", /* warning */
    "\x1B[31m", /* error */
};

FILE *LOG_FILE = NULL;
int LOG_LEVEL_SUPPRESS = 0;

void pprintf(int level, const char* format, ...)
{
    va_list args;

    if (level < 0 || level > 3)
        level = 0;

    if (level < LOG_LEVEL_SUPPRESS)
        return;

    if (!LOG_FILE)
        LOG_FILE = stderr;

    fprintf(LOG_FILE, "[%s%s\x1B[0m] ", LOG_LEVEL_COLOR[level], LOG_LEVEL[level]);
    va_start(args, format);
    vfprintf(LOG_FILE, format, args);
    va_end(args);
    fprintf(LOG_FILE, "\n");

    if (level == LOG_ERROR)
        raise(SIGABRT);
}

void print_chunk(png_chunk *chunk)
{
        switch (((png_chunk*)chunk)->type) {
        case PNG_IHDR:
            pprintf(LOG_DEBUG, "CHUNK: IHDR");
            pprintf(LOG_DEBUG, "ihdr->width: %d", ((png_ihdr*)chunk)->width);
            pprintf(LOG_DEBUG, "ihdr->height: %d", ((png_ihdr*)chunk)->height);
            pprintf(LOG_DEBUG, "ihdr->bit_depth: 0x%04x", ((png_ihdr*)chunk)->bit_depth);
            pprintf(LOG_DEBUG, "ihdr->color_type: 0x%04x", ((png_ihdr*)chunk)->color_type);
            pprintf(LOG_DEBUG, "ihdr->compression_method: 0x%04x", ((png_ihdr*)chunk)->compression_method);
            pprintf(LOG_DEBUG, "ihdr->filter_method: 0x%04x", ((png_ihdr*)chunk)->filter_method);
            pprintf(LOG_DEBUG, "ihdr->interlace_method: 0x%04x", ((png_ihdr*)chunk)->interlace_method);
            break;
        case PNG_PLTE:
            pprintf(LOG_DEBUG, "CHUNK: PLTE");
            pprintf(LOG_DEBUG, "plte->palette: @0x%p", ((png_plte*)chunk)->palette);
            pprintf(LOG_DEBUG, "plte->palette_size: 0x%08x", ((png_plte*)chunk)->palette_size);
            break;
        case PNG_IDAT:
            pprintf(LOG_DEBUG, "CHUNK: IDAT");
            pprintf(LOG_DEBUG, "idat->deflate_flag: 0x%02x", ((png_idat*)chunk)->deflate_flag);
            pprintf(LOG_DEBUG, "idat->data: @0x%p", ((png_idat*)chunk)->data);
            pprintf(LOG_DEBUG, "idat->data_size: 0x%08x", ((png_idat*)chunk)->data_size);
            break;
        case PNG_IEND:
            pprintf(LOG_DEBUG, "CHUNK: IEND");
            break;
        default:
            break;
    }
}
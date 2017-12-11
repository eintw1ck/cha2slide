#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "zlib.h"
#include "crc32.h"

int png_new_ihdr(png_ihdr **chunk, uint32_t width, uint32_t height, uint8_t bit_depth, uint8_t color_type, uint8_t interlace_method)
{
	if (!(*chunk)) (*chunk) = malloc(sizeof(png_ihdr));
	(*chunk)->type = PNG_IHDR;
	
	if (!((width & 0x7FFFFFFF) && (height & 0x7FFFFFFF)))
		return 1; /* width or height is not a 31 bit integer */
	(*chunk)->width = width;
	(*chunk)->height = height;
	if (bit_depth & (bit_depth - 1))
		return 2; /* bit depth is not a power of two */
	switch (color_type) {
	case 0:
	case 3:
		if (!(bit_depth & 0x000F) && !((color_type == 3) && (bit_depth & 0x0010)))
			return 3; /* bit depth is not 1, 2, 4, or 8 for indexed color or (also 16) for grayscale */
		break;
	case 1:
	case 2:
	case 4:
		if (!(bit_depth & 0x0018)) /* bit depth is not 8 or 16 for grayscale with alpha, truecolor or truecolor with alpha */
			return 4;
		break;
	default: /* color type is unknown */
		return 5;
	}
	(*chunk)->bit_depth = bit_depth;
	(*chunk)->color_type = color_type;
	if (interlace_method & 0xFFFE)
		return 6; /* interlace_method is not 1 or 0 */
	(*chunk)->interlace_method = interlace_method;
	
	return 0;
}

int png_new_plte(png_plte **chunk, uint8_t **palette, uint8_t palette_size)
{
	if (!(*chunk)) (*chunk) = malloc(sizeof(png_plte));
	(*chunk)->type = PNG_PLTE;
	
	(*chunk)->palette_size = palette_size;
	(*chunk)->palette = palette;
	
	return 0;
}

int png_new_idat(png_idat **chunk, uint8_t *data, uint32_t data_size, uint32_t width)
{
	if (!(*chunk)) (*chunk) = malloc(sizeof(png_idat));
	(*chunk)->type = PNG_IDAT;
	
	(*chunk)->zlib_cmf = zlib_cmf_method_deflate | zlib_cinfo(0);
	(*chunk)->zlib_flag = zlib_fcheck(((uint16_t)(*chunk)->zlib_cmf << 8) + zlib_flevel_default) & 0xFF;
	(*chunk)->deflate_flag = 0;

	png_set_idat_data((*chunk), data, data_size, width);

	return 0;
}

int png_set_idat_data(png_idat *chunk, uint8_t *data, uint32_t data_size, uint32_t width) {
	uint32_t height = data_size / (width ? width : 1);

	if (!chunk) return 1;

	chunk->data_size = data_size + height; /* include space for filter type byte for each row */
	chunk->data = malloc(chunk->data_size);

	for (int i = 0; i < (data_size/width); i++) {
		chunk->data[i*(width+1)] = 0;
		memcpy(&chunk->data[1+i*(width+1)], &data[i*width], width);
	}
	return 0;
}

int png_new_iend(png_iend **chunk)
{
	if (!(*chunk)) (*chunk) = malloc(sizeof(png_iend));
	(*chunk)->type = PNG_IEND;

	return 0;
}

int png_write_signature(FILE *file)
{
	uint8_t signature[8] = { 0x89, BYTE('P'), BYTE('N'), BYTE('G'), 0x0D, 0x0A, 0x1A, 0x0A };

	return !(fwrite(signature, 8, 1, file) == 1);
}

int png_write_ihdr(FILE *file, png_ihdr *chunk)
{
	uint32_t size = 13;

	BEGIN_CHUNK(file, &chunk->type);
	WRITE_CHUNK_BSWAP(&chunk->type, 32);
	WRITE_CHUNK_BSWAP(&chunk->width, 32);
	WRITE_CHUNK_BSWAP(&chunk->height, 32);
	WRITE_CHUNK_BSWAP(&chunk->bit_depth, 8);
	WRITE_CHUNK_BSWAP(&chunk->color_type, 8);
	WRITE_CHUNK_BSWAP(&chunk->compression_method, 8);
	WRITE_CHUNK_BSWAP(&chunk->filter_method, 8);
	WRITE_CHUNK_BSWAP(&chunk->interlace_method, 8);
	END_CHUNK(size);

	return 0;
}

int png_write_plte(FILE *file, png_plte *chunk)
{
	BEGIN_CHUNK(file, &chunk->type);

	if (chunk->palette_size) {
		for (int i = 0; i < chunk->palette_size/3; i++) {
			WRITE_CHUNK_CRC(chunk->palette[i], 3);
		}
	}

	END_CHUNK(chunk->palette_size);

	return 0;
}

int png_write_idat(FILE *file, png_idat *chunk)
{
	uint32_t size = 2;
	uint32_t adler = DEFAULT_ADLER32;

	BEGIN_CHUNK(file, &chunk->type);

	WRITE_CHUNK_BSWAP(&chunk->type, 32);
	WRITE_CHUNK_BSWAP(&chunk->zlib_cmf, 8);
	WRITE_CHUNK_BSWAP(&chunk->zlib_flag, 8);

	switch (chunk->deflate_flag) {
	case 0: {
		uint32_t remaining = chunk->data_size;
		uint32_t pos = 0;
		uint8_t dflag = chunk->deflate_flag;
		while (remaining) {
			uint16_t len;
			uint16_t nlen;
	
			if (remaining > 0xFFFF) {
				remaining -= 0xFFFF;
				len = 0xFFFF;
			} else {
				len = remaining;
				remaining = 0;
				dflag |= 0x1; /* last block */
			}
			nlen = ~len;

			WRITE_CHUNK_BSWAP(&dflag, 8);
			adler = adler32(&dflag, 1, adler);
			WRITE_CHUNK_CRC(&len, 2);
			adler = adler32(&chunk->data_size, 2, adler);
			WRITE_CHUNK_CRC(&nlen, 2);
			adler = adler32(&nlen, 2, adler);

			WRITE_CHUNK_CRC(&chunk->data[pos], len);
			size += len + 5;
			adler = adler32(&chunk->data[pos], len, adler);
			pos += len;
		}
		break;
	}
	case 1 << 1: {
		// TODO: implement compression with fixed Huffman codes
	}
	case 2 << 1: {
		// TODO: implement compression with dynamic Huffman codes
	}
	default:
		fprintf(stderr, "\x1b[31mENOSYS: compression mode for deflate not supported.\x1b[0m\n");
		exit(EXIT_FAILURE);
	}
	/* TODO: work out why adding in adler32 messes literally everything up (even when changing block size) */
	/*
	WRITE_CHUNK_CRC(&adler, 4);
	size+=4;
	*/

	END_CHUNK(size);
	return 0;
}

int png_write_iend(FILE *file, png_iend *chunk)
{
	uint8_t iend[12] = { 0x00, 0x00, 0x00, 0x00, BYTE('I'), BYTE('E'), BYTE('N'), BYTE('D'), 0xAE, 0x42, 0x60, 0x82 };
	(void)(chunk);
	return !(fwrite(iend, 12, 1, file) == 1);
}


int png_write_chunk(FILE *file, png_chunk *chunk)
{
	switch (((png_chunk*)chunk)->type) {
		case PNG_IHDR:
			return png_write_ihdr(file, (png_ihdr*)chunk);
		case PNG_PLTE:
			return png_write_plte(file, (png_plte*)chunk);
		case PNG_IDAT:
			return png_write_idat(file, (png_idat*)chunk);
		case PNG_IEND:
			return png_write_iend(file, (png_iend*)chunk);
		default:
			fprintf(stderr, "\x1b[31mEINVAL: unable to write unknown chunk type (%04x).\x1b[0m\n", ((png_chunk*)chunk)->type);
			exit(EXIT_FAILURE);
	}
}

int png_write(FILE *file, png_chunk *chunks[])
{
	png_write_signature(file);
	for (int i = 0, ret = 0; chunks[i]; i++) {
		if ((ret = png_write_chunk(file, chunks[i]))) {
			return ret;
		}
	}
	return 0;
}

#if !defined(__DEFLATE_H)
#define __DEFLATE_H

#include <stdint.h>

enum deflate_btype
{
	deflate_btype_none = 0, /* no compression */
	deflate_btype_fixed = 1, /* compressed with fixed Huffman codes */
	deflate_btype_dynamic = 2, /* compressed with dynamic Huffman codes */
	deflate_btype_error = 3, /* reserved (error) */
};

struct deflate_huffman
{

};

int deflate(void *data, size_t size);
int enflate(void *data, size_t size);

#endif /* __DEFLATE_H */

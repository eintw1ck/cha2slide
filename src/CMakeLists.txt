add_library(png_core OBJECT
	adler32.c
	blake2b.c
    crc32.c
    chacha20.c
    chunk.c
    macro.c
)

add_library(png_test OBJECT
    test.c
    test_blake2b.c
    test_chacha20.c
    test_chunk.c
    test_crc32.c
)

add_library(png_slide OBJECT
	cha2slide.c
)
set(png_core_src
    blake2b.c
    crc32.c
    chacha20.c
    chunk.c
    log.c
)
add_library(png_core OBJECT ${png_core_src})
file(COPY ${png_core_src} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(png_test_src
    test.c
    testblake2b.c
    testchacha20.c
    testchunk.c
    testcrc32.c
)
add_library(png_test OBJECT ${png_test_src})
file(COPY ${png_test_src} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_library(png_slide OBJECT
    cha2slide.c
)
file(COPY cha2slide.c DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

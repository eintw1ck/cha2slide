cmake_minimum_required (VERSION 2.8.11)
project(png)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#include(UseLATEX)
include(UseLatexMk)
message(STATUS "CMAKE_INSTALL_LIBDIR='${CMAKE_INSTALL_LIBDIR}'")

# C11
set(CMAKE_C_STANDARD 11)
# No GNU
set(CMAKE_C_EXTENSIONS OFF)

#set(CMAKE_C_FLAGS "")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -pedantic -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Werror -Wextra -pedantic")

# __APPLE_USE_RFC_3542 define if apple

find_package(ZLIB REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/munit)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(src)

# build libdmt from modules
add_library(munit OBJECT munit/munit.c)
set(CMAKE_CXX_FLAGS "-fexceptions")
set_target_properties(munit PROPERTIES COMPILE_FLAGS "-Wno-missing-field-initializers")
set_target_properties(png_test PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter")
add_executable(run_tests $<TARGET_OBJECTS:munit> $<TARGET_OBJECTS:png_core> $<TARGET_OBJECTS:png_test>)
target_link_libraries(run_tests ${ZLIB_LIBRARIES})
add_executable(cha2slide $<TARGET_OBJECTS:png_core> $<TARGET_OBJECTS:png_slide>)
target_link_libraries(cha2slide ${ZLIB_LIBRARIES})
#add_executable(dmt $<TARGET_OBJECTS:png_core>)

add_subdirectory(doc)

# enable tests
enable_testing()
# make check
# if (CMAKE_CONFIGURATION_TYPES)
#     add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} 
#         --force-new-ctest-process --output-on-failure 
#         --build-config "$<CONFIGURATION>")
# else()
#     add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} 
#         --force-new-ctest-process --output-on-failure)
# endif()
add_custom_target(check COMMAND ${CMAKE_BINARY_DIR}/run_tests DEPENDS run_tests)
add_custom_target(loc COMMAND cloc ${CMAKE_SOURCE_DIR}/src --exclude-lang=make)

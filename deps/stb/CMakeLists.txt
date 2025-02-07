cmake_minimum_required(VERSION 3.13)
project(stb)

add_library(stb STATIC
    stb_image_write.c
    stb_image.c
    stb_rect_pack.c
    stb_truetype.c
    stb_vorbis.c)
target_include_directories(stb PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
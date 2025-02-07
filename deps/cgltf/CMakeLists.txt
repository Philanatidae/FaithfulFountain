cmake_minimum_required(VERSION 3.13)
project(cgltf)

add_library(cgltf STATIC
    cgltf.c
)
target_include_directories(cgltf PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

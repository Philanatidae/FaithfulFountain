# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_executable(ff-asset-builder)
target_include_directories(ff-asset-builder PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

ff_target_link_ff_library(ff-asset-builder ff-core)
ff_target_link_ff_library(ff-asset-builder ff-support-desktop)
target_link_libraries(ff-asset-builder
    spirv-cross-core
    spirv-cross-glsl
    spirv-cross-reflect
    spirv-cross-msl
    spirv-cross-util
)

target_compile_options(ff-asset-builder PRIVATE ${FF_COMPILE_OPTIONS})

add_subdirectory(src)

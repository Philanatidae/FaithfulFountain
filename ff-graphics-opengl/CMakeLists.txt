# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(ff-graphics-opengl ${FF_LIBRARY_TYPE})
target_include_directories(ff-graphics-opengl PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

ff_target_link_ff_library(ff-graphics-opengl ff-core)
target_compile_definitions(ff-graphics-opengl PUBLIC FF_HAS_GRAPHICS_OPENGL)
set(FF_HAS_GRAPHICS_OPENGL 1 PARENT_SCOPE)

if(FF_DEV_FEATURES)
    target_compile_definitions(ff-graphics-opengl PUBLIC FF_DEV_FEATURES)
endif()

if(APPLE)
    target_link_libraries(ff-graphics-opengl "-framework OpenGL")
endif()

target_link_libraries(ff-graphics-opengl epoxy)

IF(ANDROID) # or ES
    target_compile_definitions(ff-graphics-opengl PUBLIC FF_GL_ES)
ENDIF()

target_compile_options(ff-graphics-opengl PRIVATE ${FF_COMPILE_OPTIONS})

add_subdirectory(src)

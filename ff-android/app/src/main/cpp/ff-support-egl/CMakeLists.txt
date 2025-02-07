# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(ff-support-egl ${FF_LIBRARY_TYPE})
target_include_directories(ff-support-egl PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

ff_target_link_ff_library(ff-support-egl ff-core)

if(FF_DEV_FEATURES)
    target_compile_definitions(ff-support-egl PUBLIC FF_DEV_FEATURES)
endif()

target_compile_definitions(ff-support-egl PUBLIC FF_SUPPORT_EGL_EXISTS)

if(ANDROID)
target_link_libraries(ff-android
    EGL
)
else()
    message(FATAL_ERROR "ff-support-egl currently supports Android only.")
endif()

target_compile_options(ff-support-egl PRIVATE ${FF_COMPILE_OPTIONS})

add_subdirectory(src)

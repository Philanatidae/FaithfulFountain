# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(ff-audio-oboe ${FF_LIBRARY_TYPE})
target_include_directories(ff-audio-oboe PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

ff_target_link_ff_library(ff-audio-oboe ff-core)
target_compile_definitions(ff-audio-oboe PUBLIC FF_HAS_AUDIO_OBOE)
set(FF_HAS_AUDIO_OBOE 1 PARENT_SCOPE)

target_link_libraries(ff-audio-oboe
    oboe
)

if(FF_DEV_FEATURES)
    target_compile_definitions(ff-audio-oboe PUBLIC FF_DEV_FEATURES)
endif()

target_compile_options(ff-audio-oboe PRIVATE ${FF_COMPILE_OPTIONS})

add_subdirectory(src)

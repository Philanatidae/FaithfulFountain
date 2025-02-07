# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(ff-audio-portaudio ${FF_LIBRARY_TYPE})
target_include_directories(ff-audio-portaudio PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

ff_target_link_ff_library(ff-audio-portaudio ff-core)
target_compile_definitions(ff-audio-portaudio PUBLIC FF_HAS_AUDIO_PORTAUDIO)
set(FF_HAS_AUDIO_PORTAUDIO 1 PARENT_SCOPE)

target_link_libraries(ff-audio-portaudio
    portaudio_static)

if(FF_DEV_FEATURES)
    target_compile_definitions(ff-audio-portaudio PUBLIC FF_DEV_FEATURES)
endif()

target_compile_options(ff-audio-portaudio PRIVATE ${FF_COMPILE_OPTIONS})

add_subdirectory(src)

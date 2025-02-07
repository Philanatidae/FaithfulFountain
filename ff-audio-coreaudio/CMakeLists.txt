# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(ff-audio-coreaudio ${FF_LIBRARY_TYPE})
target_include_directories(ff-audio-coreaudio PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

ff_target_link_ff_library(ff-audio-coreaudio ff-core)
target_compile_definitions(ff-audio-coreaudio PUBLIC FF_HAS_AUDIO_COREAUDIO)
set(FF_HAS_AUDIO_COREAUDIO 1 PARENT_SCOPE)

if(FF_DEV_FEATURES)
    target_compile_definitions(ff-audio-coreaudio PUBLIC FF_DEV_FEATURES)
endif()

target_link_libraries(ff-audio-coreaudio "-framework CoreFoundation")
target_link_libraries(ff-audio-coreaudio "-framework CoreAudio")
target_link_libraries(ff-audio-coreaudio "-framework AudioToolbox")
if(IOS)
target_link_libraries(ff-audio-coreaudio "-framework AVFoundation")
endif()

target_compile_options(ff-audio-coreaudio PRIVATE ${FF_COMPILE_OPTIONS})

add_subdirectory(src)

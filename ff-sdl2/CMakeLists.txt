# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

if(FF_WINDOWS_WINMAIN AND WIN32)
    add_executable(ff-sdl2 WIN32)
else()
    add_executable(ff-sdl2)
endif()

target_include_directories(ff-sdl2 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(TARGET SDL2::SDL2main)
    target_link_libraries(ff-sdl2 SDL2::SDL2main)
endif()
target_link_libraries(ff-sdl2
    epoxy
    SDL2::SDL2
)
# @todo Ensure that SDL2 is build as shared in PROD

ff_target_link_ff_library(ff-sdl2 ff-core)

add_subdirectory(src)

if(FF_DEV_FEATURES)
    target_compile_definitions(ff-sdl2 PRIVATE FF_DEV_FEATURES)
endif()

# @todo Does SDL2 add these automatically?
if(APPLE)
    target_link_libraries(ff-sdl2 "-framework Foundation")
    target_link_libraries(ff-sdl2 "-framework Cocoa")
    target_link_libraries(ff-sdl2 "-framework Quartz")
endif()

# Support
ff_target_link_ff_library(ff-sdl2 ff-support-desktop) # Required
if(FF_HAS_SUPPORT_APPLE)
    ff_target_link_ff_library(ff-sdl2 ff-support-apple)
endif()

# Graphics
if(FF_HAS_GRAPHICS_METAL)
    ff_target_link_ff_library(ff-sdl2 ff-graphics-metal)
endif()
if(FF_HAS_GRAPHICS_OPENGL)
    ff_target_link_ff_library(ff-sdl2 ff-graphics-opengl)
endif()

# Audio
if(FF_HAS_AUDIO_COREAUDIO)
    ff_target_link_ff_library(ff-sdl2 ff-audio-coreaudio)
endif()
if(FF_HAS_AUDIO_PORTAUDIO)
    ff_target_link_ff_library(ff-sdl2 ff-audio-portaudio)
endif()
if(FF_HAS_AUDIO_OBOE)
    ff_target_link_ff_library(ff-sdl2 ff-audio-oboe)
endif()

target_compile_options(ff-sdl2 PRIVATE ${FF_COMPILE_OPTIONS})

# Link game
if(FF_LINK_STATIC)
    ff_target_link_ff_library(ff-sdl2 ${FF_GAME})
endif()


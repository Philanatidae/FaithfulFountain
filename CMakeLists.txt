# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

# @todo Lockdown CMake version
cmake_minimum_required(VERSION 3.20)

if(APPLE AND NOT IOS)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15.4 CACHE STRING "Minimum OS X deployment version" FORCE)
endif()

project(FaithfulFountain
    VERSION 0.1
    DESCRIPTION "A multi-platform C++ game engine."
)

set(FF_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

# Use cache because ff-android and ff-ios include this file as a subdirectory
# instead of this file being root.
set(CMAKE_CXX_STANDARD 17 CACHE INTERNAL "CMAKE_CXX_STANDARD")
set(CMAKE_C_STANDARD 99 CACHE INTERNAL "CMAKE_C_STANDARD")

if(MSVC)
    # I hate that we even have to do this.
    add_definitions(-DNOMINMAX)
    add_definitions(-DLEAN_AND_MEAN)

    # @todo Add -DLEAN_AND_MEAN? We are including this when we are including platform_folders.h.

    # https://stackoverflow.com/a/41618677/1457856
    # On Windows, a static library is generated for symbol exports.
    # This will export everything without having to explicitly
    # mark all the functions we want to export (which is almost all
    # of them in most cases, especially ff-core).
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

# == BEGIN OPTIONS ==
option(FF_DEV_FEATURES "Enables development features." OFF)
option(FF_STD_CONSOLE_COLOR "Enable color when using the std console." ON)
option(FF_LINK_SHARED "Enable shared linking libraries." ON)
option(FF_LINK_STATIC "Enable static linking libraries." OFF)
option(FF_HARD_LINK_ENTRY "Link ff_entry to ff-core directly, instead of loading through dlsym/GetProcAddress." OFF) # Since ff_entry isn't being used directly, sometimes it can be excluded during optimization.
option(FF_ENABLE_BACKWARD "Enable Backward stack trace." ON)

option(FF_APPLE_USE_NSBUNDLE "Use NSBundle for loading assets (used in production)." OFF)
option(FF_APPLE_UNIVERSAL_2_IN_RELEASE "On macOS, when configured for Release, build Universal 2 binaries." ON)

option(FF_WINDOWS_WINMAIN "Use WinMain when compiling for Windows." OFF)

option(FF_BUILD_GRAPHICS_METAL "Enables building ff-graphics-metal." ON)
option(FF_BUILD_GRAPHICS_OPENGL "Enables building ff-graphics-opengl." ON)

option(FF_BUILD_AUDIO_COREAUDIO "Enables building ff-audio-coreaudio." ON)
option(FF_BUILD_AUDIO_PORTAUDIO "Enables building ff-audio-portaudio." ON)
option(FF_BUILD_AUDIO_OBOE "Enables building ff-audio-oboe." ON)

set(FF_MESSAGE_MAX_MEMBER_VARIABLES 6 CACHE STRING "Maximum member variables allowed in Message definitions (increasing generates additional macros).")

set(FF_GAME "" CACHE STRING "Specifies the game folder and library name (required).")
if(FF_GAME STREQUAL "") 
    message(FATAL_ERROR "FF_GAME is required.")
endif()
#== END OPTIONS ==

if(FF_LINK_SHARED)
    set(FF_LIBRARY_TYPE SHARED)
    if(FF_LINK_STATIC)
        set(FF_LINK_STATIC OFF)
    endif()

    if(FF_HARD_LINK_ENTRY)
        message(FATAL_ERROR "FF_HARD_LINK_ENTRY requires static linking to be enabled.")
    endif()
elseif(FF_LINK_STATIC)
    if(MSVC)
        set(FF_LIBRARY_TYPE OBJECT)
    else()
        #set(FF_LIBRARY_TYPE STATIC)
        set(FF_LIBRARY_TYPE OBJECT)
    endif()
else()
    message(FATAL_ERROR "Must choose to link libraries statically/shared.")
endif()

# @todo There's no reason to infer. Just change this to a required option.
set(FF_INFERRED_PLATFORM "Desktop")
if(IOS)
    set(FF_INFERRED_PLATFORM "iOS")
endif()
if(ANDROID)
    set(FF_INFERRED_PLATFORM "Android")
endif()

set(FF_PLATFORM ${FF_INFERRED_PLATFORM} CACHE STRING "Target platform that is being compiled (Desktop, iOS, Android).")

# @todo I believe that this is obsolete. ff doesn't care what platform you're on. Per-platform changes should be detected at runtime via environment.
string(TOLOWER ${FF_PLATFORM} FF_PLATFORM_lower)
if(FF_PLATFORM_lower STREQUAL "desktop")
    set(FF_IS_DESKTOP YES)
    set(FF_IS_MOBILE NO)
endif()
if(FF_PLATFORM_lower STREQUAL "ios" OR FF_PLATFORM_lower STREQUAL "android")
    set(FF_IS_DESKTOP NO)
    set(FF_IS_MOBILE YES)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Configuring as DEBUG.")
else()
    set(CMAKE_BUILD_TYPE "Release")
    message("Configuring as RELEASE. Use `-DCMAKE_BUILD_TYPE=Debug` for DEBUG.")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(FF_IS_RELEASE Yes)

    # @todo Not multi-platform, surely there's a better way to do this (if it's even appropriate to be here in the first place).
    message("Setting optimization to -O1.")
    set(CMAKE_CXX_FLAGS_RELEASE "-O1")

    if(APPLE)
        if(NOT IOS)
            if(FF_APPLE_UNIVERSAL_2_IN_RELEASE)
                message("macOS detected; building for Universal 2.")
                set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
            else()
                message("macOS detected, but Universal 2 builds were manually disabled. Ensure Universal 2 builds are enabled before distributing.")
            endif()
        endif()
    endif()
endif()

# Custom function for linking FF libraries,
# since we either have to static link whole
# libraries, or copy DLLs to the output.
#
# Only needed for FF libraries, the game library
# does not need this since the FF libraries are
# linked with the platform target (ff-desktop, etc.)
function(ff_target_link_ff_library target_NAME ff_lib_name)
    target_link_libraries(${target_NAME} ${ff_lib_name})

    if(FF_LINK_SHARED AND WIN32)
        message(DLL copy needs to be re-implemented)
        #get_target_property(target_TYPE ${target_NAME} TYPE)
        #if (target_TYPE STREQUAL "EXECUTABLE")
        #    # We need to copy the DLL to the executable folder
        #    add_custom_command(TARGET ${target_NAME} POST_BUILD 
        #        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        #        $<TARGET_FILE:${ff_lib_name}>
        #        $<TARGET_FILE_DIR:${target_NAME}>)
        #endif()
    endif()
endfunction()

if(MSVC)
    #set(FF_COMPILE_OPTIONS /w)
else()
    if(NOT ANDROID) # TODO: I'd like to remove this
        #set(FF_COMPILE_OPTIONS -Werror -Wall
        #    -Wno-reorder-ctor
        #    -Wno-unused-variable
        #    -Wno-unused-private-field
        #)
    endif()
endif()

add_subdirectory(deps)

add_subdirectory(ff-core)

# Graphics
if(FF_BUILD_GRAPHICS_METAL)
    if(APPLE)
        add_subdirectory(ff-graphics-metal)
    endif()
endif()
if(FF_BUILD_GRAPHICS_OPENGL)
    if(ANDROID OR APPLE OR WIN32)
        if(NOT IOS)
            add_subdirectory(ff-graphics-opengl)
        endif()
    endif()
endif()

# Audio
if(FF_BUILD_AUDIO_COREAUDIO)
    if(APPLE)
        add_subdirectory(ff-audio-coreaudio)
    endif()
endif()
if(FF_BUILD_AUDIO_PORTAUDIO)
    if(APPLE OR WIN32)
        if(NOT IOS)
            add_subdirectory(ff-audio-portaudio)
        endif()
    endif()
endif()
if(FF_BUILD_AUDIO_OBOE)
    if(ANDROID)
        add_subdirectory(ff-audio-oboe)
    endif()
endif()

# Support
if(APPLE)
    add_subdirectory(ff-support-apple)
endif()
if(FF_IS_DESKTOP)
    add_subdirectory(ff-support-desktop)
endif()

# Game
add_subdirectory("${CMAKE_SOURCE_DIR}/${FF_GAME}" "${CMAKE_BINARY_DIR}/${FF_GAME}")

# Platforms
if(FF_IS_DESKTOP)
    # @todo Might be re-named back to ff-desktop
    add_subdirectory(ff-sdl2)
endif()

# Asset Builder (depends on ff-support-desktop)
if(FF_IS_RELEASE)
    message("Asset builder is not built in Release mode.")
else()
    if(FF_IS_DESKTOP)
        add_subdirectory(ff-asset-builder)
    endif()
endif()


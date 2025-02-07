# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

project(ff-android)
cmake_minimum_required(VERSION 3.20)

# Check if CMAKE_ANDROID_ARCH_ABI is set to a supported value
if(NOT CMAKE_ANDROID_ARCH_ABI MATCHES "^(x86|x86_64|arm64-v8a|armeabi-v7a)$")
    message(FATAL_ERROR "ANDROID_ABI must be set to one of the following: x86, x86_64, arm64-v8a, armeabi-v7a")
endif()

# @todo Move to all standard CMake Android variables
message(STATUS "Building for Android architecture: ${CMAKE_ANDROID_ARCH_ABI}")
message(STATUS "Android API level: ${ANDROID_NATIVE_API_LEVEL}")
message(STATUS "Android NDK root: ${CMAKE_ANDROID_NDK}")
message(STATUS "Android STL: ${CMAKE_ANDROID_STL_TYPE}")
message(STATUS "Android ARM Mode: ${CMAKE_ANDROID_ARM_MODE}")
message(STATUS "Android ARM NEON: ${CMAKE_ANDROID_ARM_NEON}")
message(STATUS "Android sysroot: ${CMAKE_SYSROOT}")
message(STATUS "Android toolchain root: ${ANDROID_TOOLCHAIN_ROOT}")
message(STATUS "C Flags: ${CMAKE_C_FLAGS}")
message(STATUS "CXX flags: ${CMAKE_CXX_FLAGS}")

# Faithful Fountain
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(FF_DEV_FEATURES OFF)
else()
    set(FF_DEV_FEATURES ON)
endif()
set(FF_LINK_SHARED OFF CACHE BOOL "" FORCE)
set(FF_LINK_STATIC ON CACHE BOOL "" FORCE)
set(FF_HARD_LINK_ENTRY ON CACHE BOOL "" FORCE)
set(FF_ENABLE_BACKWARD OFF CACHE BOOL "" FORCE)
# Make sure FF is added before any targets, as it is defining the C++ version
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../.. ${CMAKE_CURRENT_BINARY_DIR}/ff) # FF

# Android
find_library(log-lib log)

add_library(native_app_glue OBJECT # Must be OBJECT, if it is STATIC then the native activity entry will be optimized away when linking the shared library
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
        )
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue
        )

# Application
add_library(ff-android SHARED
    ff_android.cpp
    AndroidAssetBundle.cpp
    AndroidBinaryReader.cpp
    AndroidEnvironment.cpp
    AndroidGameLoop.cpp
    AndroidMotionEventTransmitter.cpp
    CVarDefaults.cpp
    LogcatConsole.cpp
)
#target_link_options(ff-android PUBLIC
#    -u ANativeActivity_onCreate # Keeps `ANativeActivity_onCreate` in the library.
#)

target_link_libraries(ff-android
    android
    native_app_glue
    ${log-lib}
    EGL
    GLESv3
)

# Libraries
add_subdirectory(ff-support-egl)

# @todo These libraries need to be linked conditionally like ff-desktop
ff_target_link_ff_library(ff-android ff-core)
ff_target_link_ff_library(ff-android ff-support-egl)
ff_target_link_ff_library(ff-android ff-graphics-opengl)
ff_target_link_ff_library(ff-android ff-audio-oboe)

# Link game
ff_target_link_ff_library(ff-android ${FF_GAME})


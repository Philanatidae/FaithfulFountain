# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

# Reference: https://github.com/sheldonth/ios-cmake

cmake_minimum_required(VERSION 3.14)

set(DEVELOPMENT_PROJECT_NAME "ff-ios")
set(DEVELOPMENT_TEAM_ID "TDMAA5AB9L")
set(APP_NAME "RollGame (TBD)")
set(APP_EXECUTABLE "FaithfulFountain")
set(APP_ICON "AppIcon")
set(APP_BUNDLE_IDENTIFIER "com.philiprader.FFRollGame")
set(APP_VERSION "0.1.0")
set(APP_BUNDLE_VERSION "1")
set(CODE_SIGN_IDENTITY "iPhone Developer")

set(IOS_DEPLOYMENT_TARGET 13.0) # Minimum iOS/iPadOS version
set(MACOS_DEPLOYMENT_TARGET 11.0.0) # Minimum version of macOS that app can run on. 11.0 is Big Sur, first to support running iOS apps.
set(DEVICE_FAMILY "1") # 1 for iPhone, 2 for iPad, 1,2 for both
set(LOGIC_ONLY_TESTS 0)

project(${DEVELOPMENT_PROJECT_NAME})
set(CMAKE_CXX_STANDARD 14)

###############
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/Modules/")

#set(FF_DEV_FEATURES ON)
set(FF_LINK_SHARED OFF CACHE BOOL "" FORCE)
set(FF_LINK_STATIC ON CACHE BOOL "" FORCE)
set(FF_HARD_LINK_ENTRY ON CACHE BOOL "" FORCE)
set(FF_ENABLE_BACKWARD OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/ff) # FF
###############

#include(BundleUtilities)

set(PRODUCT_NAME ${APP_NAME})
set(EXECUTABLE_NAME ${APP_EXECUTABLE})
set(MACOSX_BUNDLE_EXECUTABLE_NAME ${EXECUTABLE_NAME})
set(MACOSX_BUNDLE_INFO_STRING ${APP_BUNDLE_IDENTIFIER})
set(MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})
set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_BUNDLE_IDENTIFIER})
set(MACOSX_BUNDLE_ICON_FILE "AppIcon")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${APP_BUNDLE_VERSION})
set(MACOSX_BUNDLE_BUNDLE_VERSION ${APP_VERSION})
set(MACOSX_BUNDLE_COPYRIGHT "2022 Philip Rader")
set(MACOSX_DEPLOYMENT_TARGET ${MACOS_DEPLOYMENT_TARGET}) # macOS deployment target
set(CMAKE_OSX_DEPLOYMENT_TARGET ${IOS_DEPLOYMENT_TARGET} CACHE STRING "" FORCE) # iOS deployment target
set(DEPLOYMENT_TARGET ${IOS_DEPLOYMENT_TARGET} CACHE STRING "" FORCE) # Needed by the toolchain we're using

set(APP_HEADER_FILES
  ./AppDelegate.h
  ./ViewController.h
)

set(APP_SOURCE_FILES
  ./AppDelegate.m
  ./CVarDefaults.cpp
  ./iOSEnvironment.mm
  ./iOSGameLoop.mm
  ./iOSMotionEventTransmitter.mm
  ./main.m
  ./ViewController.mm
)

set(RESOURCES
  ./Main.storyboard
  ./LaunchScreen.storyboard
  ./Assets.xcassets
)

add_executable(
    ${EXECUTABLE_NAME}
    MACOSX_BUNDLE
    ${APP_HEADER_FILES}
    ${APP_SOURCE_FILES}
    ${RESOURCES}
)

##################
ff_target_link_ff_library(${EXECUTABLE_NAME} ff-core)
ff_target_link_ff_library(${EXECUTABLE_NAME} ff-graphics-metal)
ff_target_link_ff_library(${EXECUTABLE_NAME} ff-audio-coreaudio)
ff_target_link_ff_library(${EXECUTABLE_NAME} ff-support-apple)

ff_target_link_ff_library(${EXECUTABLE_NAME} ${FF_GAME})
##################

# Locate system libraries on iOS
find_library(UIKIT UIKit)
find_library(FOUNDATION Foundation)
find_library(MOBILECORESERVICES MobileCoreServices)
find_library(CFNETWORK CFNetwork)
find_library(SYSTEMCONFIGURATION SystemConfiguration)
find_library(COREMOTION CoreMotion)

# link the frameworks located above
target_link_libraries(${EXECUTABLE_NAME} ${UIKIT})
target_link_libraries(${EXECUTABLE_NAME} ${FOUNDATION})
target_link_libraries(${EXECUTABLE_NAME} ${MOBILECORESERVICES})
target_link_libraries(${EXECUTABLE_NAME} ${CFNETWORK})
target_link_libraries(${EXECUTABLE_NAME} ${SYSTEMCONFIGURATION})
target_link_libraries(${EXECUTABLE_NAME} ${COREMOTION})

# Turn on ARC
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")

# Create the app target
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
                      XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
                      RESOURCE "${RESOURCES}"
                      XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${IOS_DEPLOYMENT_TARGET}
                      XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${CODE_SIGN_IDENTITY}
                      XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${DEVELOPMENT_TEAM_ID}
                      XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY ${DEVICE_FAMILY}
                      MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/plist.in
                      XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
                      XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES NO
                      XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)"
                      XCODE_ATTRIBUTE_ENABLE_TESTABILITY YES
                      XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN YES
                      XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${APP_BUNDLE_IDENTIFIER}
                      XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME ${APP_ICON}
)

# Set the app's linker search path to the default location on iOS
set_target_properties(
    ${EXECUTABLE_NAME}
    PROPERTIES
    XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS
    "@executable_path/Frameworks"
)

# Copy assets into the app bundle
# TODO: Make asset builder to run from CMake and generates directly into the app bundle

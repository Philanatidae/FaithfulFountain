# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(ff-support-apple ${FF_LIBRARY_TYPE})
target_include_directories(ff-support-apple PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

ff_target_link_ff_library(ff-support-apple ff-core)
target_compile_definitions(ff-support-apple PUBLIC FF_HAS_SUPPORT_APPLE)
set(FF_HAS_SUPPORT_APPLE 1 PARENT_SCOPE)

if(FF_DEV_FEATURES)
    target_compile_definitions(ff-support-apple PUBLIC FF_DEV_FEATURES)
endif()

if(FF_APPLE_USE_NSBUNDLE)
    target_compile_definitions(ff-support-apple PUBLIC FF_APPLE_USE_NSBUNDLE)
endif()

target_link_libraries(ff-support-apple "-framework Foundation")
target_link_libraries(ff-support-apple "-framework Security")
if(IOS)
target_link_libraries(ff-support-apple "-framework UIKit")
else()
target_link_libraries(ff-support-apple "-framework AppKit")
endif()

target_compile_options(ff-support-apple PRIVATE ${FF_COMPILE_OPTIONS})

add_subdirectory(src)

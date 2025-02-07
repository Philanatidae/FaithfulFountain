# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(ff-support-desktop ${FF_LIBRARY_TYPE})
target_include_directories(ff-support-desktop PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

ff_target_link_ff_library(ff-support-desktop ff-core)
target_compile_definitions(ff-support-desktop PUBLIC FF_HAS_SUPPORT_DESKTOP)
set(FF_HAS_SUPPORT_DESKTOP 1 PARENT_SCOPE)

if(APPLE)
    ff_target_link_ff_library(ff-support-desktop ff-support-apple)
else()
    target_link_libraries(ff-support-desktop
        platform_folders
    )
endif()

if(FF_DEV_FEATURES)
    target_compile_definitions(ff-support-desktop PUBLIC FF_DEV_FEATURES)
endif()

if(APPLE)
target_link_libraries(ff-support-desktop "-framework Foundation")
endif()

target_compile_options(ff-support-desktop PRIVATE ${FF_COMPILE_OPTIONS})

add_subdirectory(src)

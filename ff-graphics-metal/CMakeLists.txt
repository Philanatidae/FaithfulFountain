# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(ff-graphics-metal ${FF_LIBRARY_TYPE})
target_include_directories(ff-graphics-metal PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

ff_target_link_ff_library(ff-graphics-metal ff-core)
target_compile_definitions(ff-graphics-metal PUBLIC FF_HAS_GRAPHICS_METAL)
set(FF_HAS_GRAPHICS_METAL 1 PARENT_SCOPE)

if(FF_DEV_FEATURES)
    target_compile_definitions(ff-graphics-metal PUBLIC FF_DEV_FEATURES)
endif()

target_link_libraries(ff-graphics-metal "-framework CoreFoundation")
target_link_libraries(ff-graphics-metal "-framework Metal")
if(IOS)
    target_link_libraries(ff-graphics-metal "-framework QuartzCore")
endif()
if(NOT IOS)
    target_link_libraries(ff-graphics-metal "-framework Cocoa")
    target_link_libraries(ff-graphics-metal "-framework Quartz")
endif()

target_compile_options(ff-graphics-metal PRIVATE ${FF_COMPILE_OPTIONS})

add_subdirectory(src)

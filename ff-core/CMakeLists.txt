# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_library(ff-core ${FF_LIBRARY_TYPE})
target_include_directories(ff-core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(ff-core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>)

target_link_libraries(ff-core
    cgltf
    glm
    imgui
    imguizmo
    imnodes
    magic_enum
    nlohmann_json
    pugixml
    stb
    tinyformat
    timer_lib
    #tracy @todo Re-enable Tracy
    )

if(FF_IS_RELEASE)
    target_compile_definitions(ff-core PUBLIC FF_IS_RELEASE)
endif()

if(FF_DEV_FEATURES)
    target_compile_definitions(ff-core PUBLIC FF_DEV_FEATURES)

    if(FF_ENABLE_BACKWARD)
        target_sources(ff-core PRIVATE
            ${BACKWARD_ENABLE}
        )

        add_backward(ff-core)

        target_compile_definitions(ff-core PUBLIC FF_ENABLE_BACKWARD)
    endif()

    if(NOT FF_IS_RELEASE)
        target_link_libraries(ff-core
            debugbreak
            )
    endif()

    # TODO: Add option for enabling Tracy
    #target_compile_definitions(ff-core PUBLIC TRACY_ENABLE)
endif()
if(FF_STD_CONSOLE_COLOR)
    target_compile_definitions(ff-core PUBLIC FF_STD_CONSOLE_COLOR)
endif()

# Ideally ff-core shouldn't care, but there is platform specific code that I don't want to abstract into backends yet
if(FF_IS_DESKTOP)
    target_compile_definitions(ff-core PUBLIC FF_IS_DESKTOP)
endif()
if(FF_IS_MOBILE)
    target_compile_definitions(ff-core PUBLIC FF_IS_MOBILE)
endif()

if(FF_LINK_STATIC)
    target_compile_definitions(ff-core PUBLIC FF_STATIC_GAME_ENTRY)
endif()
if(FF_HARD_LINK_ENTRY)
    target_compile_definitions(ff-core PUBLIC FF_HARD_LINK_ENTRY)
endif()

if(APPLE)
    target_link_libraries(ff-core "-framework CoreFoundation")
endif()

target_compile_options(ff-core PRIVATE ${FF_COMPILE_OPTIONS})

# Generated files
include(include/ff/messages/GenerateCmdHelpers.cmake)
include(include/ff/messages/GenerateEvent.cmake)

add_subdirectory(src)

if(FF_IS_DESKTOP)
    add_subdirectory(tests)
endif()

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/get_cpm.cmake")

macro(ff_AddRepoDepNoBuild dep_name dep_version dep_type dep_repo dep_tag)
    CPMAddPackage(
        NAME ${dep_name}
        VERSION ${dep_version}
        ${dep_type}_REPOSITORY ${dep_repo}
        ${dep_type}_TAG ${dep_tag}
        DOWNLOAD_ONLY YES
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${dep_name}
    )
    if(NOT ${dep_name}_ADDED)
        message(FATAL_ERROR "Could not add ${dep_name}.")
    endif()
endmacro()

###############################################
# tinyformat
###############################################
ff_AddRepoDepNoBuild(tinyformat 2.3.0 GIT "https://github.com/c42f/tinyformat" 705e3f4)

add_library(tinyformat INTERFACE)
target_include_directories(tinyformat SYSTEM INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

###############################################
# PlatformFolders
###############################################
if(NOT IOS AND NOT ANDROID)
    # @todo Remove dependency on PlatformFolders. We are exeptionally close, we only need PlatformFolders on Windows/Linux.

    ff_AddRepoDepNoBuild(PlatformFolders 4.2.0 GIT "https://github.com/sago007/PlatformFolders" 784f8ce)

    set(PLATFORMFOLDERS_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(PLATFORMFOLDERS_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(PlatformFolders)
endif()

###############################################
# Catch 2
###############################################
if(NOT IOS AND NOT ANDROID)
    ff_AddRepoDepNoBuild(Catch2 3.3.1 GIT "https://github.com/catchorg/Catch2" 2ab20a0)

    add_subdirectory(Catch2)
endif()

###############################################
# SDL2
###############################################
if(NOT IOS AND NOT ANDROID)
    ff_AddRepoDepNoBuild(SDL 2.26.4 GIT "https://github.com/libsdl-org/SDL" 07d0f51)

    add_subdirectory(SDL)
endif()

###############################################
# libepoxy
###############################################
if(NOT IOS)
    ff_AddRepoDepNoBuild(libepoxy-cmake 1.5.10-rev2 GIT "https://github.com/Philanatidae/libepoxy-cmake" a375152)

    set(BUILD_SHARED_LIBS NO)
    # @todo Instead of excluding, have an inclusive for X (Linux, BSD, etc.)
    if(NOT ANDROID AND NOT APPLE AND NOT WIN32)
        # Linux
        set(EPOXY_ENABLE_GLX Yes)
        set(EPOXY_ENABLE_X11 Yes)
    endif()
    if(ANDROID)
        set(EPOXY_ENABLE_EGL Yes)
    endif()
    add_subdirectory(libepoxy-cmake)
endif()

###############################################
# GLM
###############################################
ff_AddRepoDepNoBuild(glm 0.9.9.8 GIT "https://github.com/g-truc/glm" bf71a83)

add_library(glm INTERFACE)
target_include_directories(glm SYSTEM INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glm>)
target_compile_definitions(glm INTERFACE GLM_FORCE_DEPTH_ZERO_TO_ONE)

###############################################
# STB
###############################################
add_subdirectory(stb)

###############################################
# Nlohmann (JSON)
###############################################
ff_AddRepoDepNoBuild(nlohmann_json 3.11.2 GIT "https://github.com/nlohmann/json" bc889af)

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json SYSTEM INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/nlohmann_json/include>)

###############################################
# pugixml
###############################################
ff_AddRepoDepNoBuild(pugixml 1.13 GIT "https://github.com/zeux/pugixml" a0e0643)

add_subdirectory(pugixml)

###############################################
# DirectX
###############################################
#if(WIN32)
#    find_package(DirectX)
#endif()

###############################################
# PortAudio
###############################################
if(NOT IOS AND NOT ANDROID)
    # PortAudio v20 not released yet.
    ff_AddRepoDepNoBuild(portaudio 20 GIT "https://github.com/PortAudio/portaudio" 52b1026)

    add_subdirectory(portaudio)
endif()

###############################################
# Backward
###############################################
ff_AddRepoDepNoBuild(backward 1.6 GIT "https://github.com/bombela/backward-cpp" 3bb9240)

add_subdirectory(backward)

###############################################
# SPIRV-cross
###############################################
if(NOT IOS AND NOT ANDROID)
    # Releases are tags with IEEE 
    ff_AddRepoDepNoBuild(SPIRV-cross 20210115 GIT "https://github.com/KhronosGroup/SPIRV-Cross" 9acb9ec)

    set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_CPP OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_C_API OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_SKIP_INSTALL ON CACHE BOOL "" FORCE)

    set(SPIRV_CROSS_ENABLE_HLSL OFF CACHE BOOL "" FORCE) # Turned off for now, can re-enable when DX support is added
    add_subdirectory(SPIRV-cross)
endif()

###############################################
# Oboe
###############################################
if(ANDROID)
    ff_AddRepoDepNoBuild(oboe 1.7.0 GIT "https://github.com/google/oboe" 8740d0f)

    add_subdirectory(oboe)
endif()

###############################################
# Tracy
###############################################
if(APPLE OR WIN32)
    ff_AddRepoDepNoBuild(tracy 0.9 GIT "https://github.com/wolfpld/tracy" 5a1f537)

    add_subdirectory(tracy)
endif()

###############################################
# debugbreak
###############################################
ff_AddRepoDepNoBuild(debugbreak 1.0 GIT "https://github.com/scottt/debugbreak" 56b6b8f)

add_library(debugbreak INTERFACE)
target_include_directories(debugbreak SYSTEM INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

###############################################
# cgltf
###############################################
add_subdirectory(cgltf)

################################################
## Bullet
################################################
#
## Reference: https://github.com/bulletphysics/bullet3/issues/3167
#ff_AddRepoDepNoBuild(Bullet 3.2.4 GIT "https://github.com/bulletphysics/bullet3" 7dee343)
#
#
#set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
#set(BUILD_ENET OFF CACHE BOOL "" FORCE)
#set(BUILD_CLSOCKET OFF CACHE BOOL "" FORCE)
#set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
#set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
#set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
#set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)
#set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
#add_subdirectory(Bullet)
#
#add_library(Bullet INTERFACE)
#target_include_directories(Bullet SYSTEM INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Bullet/src>)
#target_link_libraries(Bullet INTERFACE Bullet3Common BulletDynamics LinearMath BulletCollision)

################################################
## ReactPhysics3D
################################################

ff_AddRepoDepNoBuild(reactphysics3d 0.9.0 GIT "https://github.com/DanielChappuis/reactphysics3d" 4bbbaa7)

set(RP3D_COMPILE_TESTBED OFF CACHE BOOL "" FORCE)
set(RP3D_COMPILE_TESTS OFF CACHE BOOL "" FORCE)
set(RP3D_PROFILING_ENABLED OFF CACHE BOOL "" FORCE)
set(RP3D_CODE_COVERAGE_ENABLED OFF CACHE BOOL "" FORCE)
set(RP3D_DOUBLE_PRECISION_ENABLED OFF CACHE BOOL "" FORCE)
add_subdirectory(reactphysics3d)

###############################################
# Dear IMGUI
###############################################
ff_AddRepoDepNoBuild(imgui 1.89.2 GIT "https://github.com/ocornut/imgui" d7c8516)

add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(imgui SYSTEM PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui>)
target_include_directories(imgui SYSTEM PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/cpp>)

###############################################
# ImGuizmo
###############################################
ff_AddRepoDepNoBuild(imguizmo 1.83 GIT "https://github.com/CedricGuillemet/ImGuizmo" 664cf2d)

add_library(imguizmo STATIC
    #imguizmo/GraphEditor.cpp
    imguizmo/ImCurveEdit.cpp
    imguizmo/ImGradient.cpp
    imguizmo/ImGuizmo.cpp
    #imguizmo/ImSequencer.cpp
)
target_link_libraries(imguizmo PUBLIC imgui)
target_include_directories(imguizmo SYSTEM PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imguizmo>)

###############################################
# timer_lib
###############################################
ff_AddRepoDepNoBuild(timer_lib 1.83 GIT "https://github.com/mjansson/timer_lib" cef1be5)

add_library(timer_lib STATIC
    timer_lib/timer.c
)
target_include_directories(timer_lib SYSTEM PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

###############################################
# magic_enum
###############################################
ff_AddRepoDepNoBuild(magic_enum 0.8.2 GIT "https://github.com/Neargye/magic_enum" e1a68e9)

add_subdirectory(magic_enum)

###############################################
# ImNodes
###############################################
ff_AddRepoDepNoBuild(imnodes 20220321 GIT "https://github.com/Philanatidae/ImNodes" 50f8458)

add_library(imnodes STATIC
    imnodes/ImNodes.cpp
    imnodes/ImNodesEz.cpp
)
target_link_libraries(imnodes PUBLIC imgui)
target_include_directories(imnodes SYSTEM PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)


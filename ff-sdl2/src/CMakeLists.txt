# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

target_sources(ff-sdl2 PRIVATE
    CVarDefaults.cpp
    SDLConsole.cpp
    SDLEnvironment.cpp
    SDLGameLoop.cpp
    SDLGamepadManager.cpp
    SDLWindow.cpp
)

if(APPLE)    
    target_sources(ff-sdl2 PRIVATE
        main.mm
        SDLEnvironment.mm
    )
else()
    target_sources(ff-sdl2 PRIVATE
        main.cpp
    )
endif()

add_subdirectory(directors)


# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

target_sources(ff-core PRIVATE
    CommandLineOptions.cpp
    Console.cpp
    Console.cpp
    CVarDefaults.cpp
    CVars.cpp
    Game.cpp
    GameServicer.cpp
    Locator.cpp
)

if(APPLE)
target_sources(ff-core PRIVATE
    Console.apple.mm
)

# TODO: I may want to move this outside of ff-core to make it remain pure C++.
# Currently the only Apple-specific bit is using NSLog instead of std::printf.
# This could be moved into the Environment for how StdConsole should print.
target_link_libraries(ff-core "-framework Foundation")
endif()

add_subdirectory(actors)
add_subdirectory(animation)
add_subdirectory(assets)
add_subdirectory(audio)
add_subdirectory(common)
add_subdirectory(components)
add_subdirectory(debug)
add_subdirectory(dev)
add_subdirectory(directors)
add_subdirectory(env)
add_subdirectory(graphics)
add_subdirectory(io)
add_subdirectory(math)
add_subdirectory(messages)
add_subdirectory(processes)
add_subdirectory(render)
add_subdirectory(systems)
add_subdirectory(util)

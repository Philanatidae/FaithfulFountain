# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_executable(ff-tests-core)

target_link_libraries(ff-tests-core
    ff-core
    Catch2::Catch2WithMain
)

target_sources(ff-tests-core PRIVATE
    entry.cpp
    
)

target_compile_options(ff-tests-core PRIVATE ${FF_COMPILE_OPTIONS})

add_subdirectory(actors)
add_subdirectory(messages)
add_subdirectory(processes)
add_subdirectory(resources)
add_subdirectory(util)

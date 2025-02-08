/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-sdl2/SDLEnvironment.hpp>

#include <ff/CVars.hpp>
#import <Foundation/Foundation.h>

namespace ff {

std::string SDLEnvironment::getConfigDirectory() const {
    // Don't want to use Platform Folders for macOS because
    // he hard-coded the path to the library, which doesn't work
    // when the application is sandboxed. Makes sense because
    // this isn't a pure C++ solution, but since we
    // have to use Objective-C for Metal this is actually
    // a really good solution.
    // 
    // NSHomeDirectory will respect sandbox.
    return std::filesystem::path([NSHomeDirectory() UTF8String])/"Library"/"Application Support"/CVars::get<std::string>("package_prefix")/CVars::get<std::string>("game_name");
}

}

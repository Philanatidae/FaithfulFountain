/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/Console.hpp>

#import <Foundation/Foundation.h>

namespace ff {
    void StdConsole::printLine(char const * const msg, char const * const color) {
        NSLog(@"%@%@%@", @(color), @(msg), @(StdConsoleColor::NO_COLOR));
    }
    void StdConsole::printLine(char const * const msg) {
        NSLog(@"%@", @(msg));
    }
}

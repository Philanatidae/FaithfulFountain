/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "LogcatConsole.hpp"

#include <android/log.h>

namespace ff {
    LogcatConsole::~LogcatConsole() {
    }

    void LogcatConsole::logImpl(const std::string& msg) {
        __android_log_print(ANDROID_LOG_INFO, "ff-android", "%s", msg.c_str());
    }
    void LogcatConsole::warnImpl(const std::string& msg) {
        __android_log_print(ANDROID_LOG_INFO, "ff-android", "%s", msg.c_str());
    }
    void LogcatConsole::errorImpl(const std::string& msg) {
        __android_log_print(ANDROID_LOG_INFO, "ff-android", "%s", msg.c_str());
    }
}

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/Console.hpp>
#include <sstream>

#ifdef FF_DEV_FEATURES
#ifdef FF_ENABLE_BACKWARD
#include <backward/backward.hpp>
#endif

#include <debugbreak/debugbreak.h>
#endif

#if !defined(__APPLE__)
#include <cstdio>
#endif

#include <ff/CVars.hpp>

namespace ff {
    Console::~Console() {
    }
    void Console::printStackTrace() {
#ifdef FF_DEV_FEATURES
#ifdef FF_ENABLE_BACKWARD
        backward::StackTrace st;
        st.load_here(10);
        backward::Printer p;
        std::ostringstream ss;
        p.print(st); // I'd prefer to use our console but Backward will put some formatting here that I don't want to mess with
#endif
#endif
    }

    void Console::attemptToBreakDebugger() {
#if defined(FF_DEV_FEATURES)
        FF_CONSOLE_LOG("Debugger break attempt.");
        if(!CVars::get<bool>("debug_break_on_assert")) {
            return;
        }
        debug_break();
#endif
    }

    std::unique_ptr<Console>& Console::getConsoleInstance() {
        static std::unique_ptr<Console> console(new StdConsole());
        return console;
    }
    std::vector<ConsoleLogEntry>& Console::getLog() {
        static std::vector<ConsoleLogEntry> log;
        return log;
    }

    void Console::addLogEntry(ConsoleLevel const& level, std::string const& msg) {
        ConsoleLogEntry entry;
        entry.level = level;
        entry.timestamp = getCurrentTimestamp();
        entry.message = msg;
        getLog().push_back(entry);

        if(getLog().size() > CVars::get<int>("debug_console_log_limit")) {
            getLog().erase(getLog().begin(),
                getLog().begin() + (getLog().size() - CVars::get<int>("debug_console_log_limit")));
        }
    }

    StdConsole::~StdConsole() {
    }

    void StdConsole::logImpl(const std::string& msg) {
        // @todo Potentially change STD color to a CVar?
#if defined(FF_STD_CONSOLE_COLOR)
        printLine(msg.c_str(), StdConsoleColor::LIGHT_CYAN);
#else
        printLine(msg.c_str());
#endif
    }
    void StdConsole::warnImpl(const std::string& msg) {
#if defined(FF_STD_CONSOLE_COLOR)
        printLine(msg.c_str(), StdConsoleColor::YELLOW);
#else
        printLine(msg.c_str());
#endif
    }
    void StdConsole::errorImpl(const std::string& msg) {
#if defined(FF_STD_CONSOLE_COLOR)
        printLine(msg.c_str(), StdConsoleColor::LIGHT_RED);
#else
        printLine(msg.c_str());
#endif
    }

#if !defined(__APPLE__)
    // Implementation for non-Apple, see Console.mm
    void StdConsole::printLine(char const * const msg) {
#if WIN32
        std::printf("%s\r\n", msg);
#else
        std::printf("%s\n", msg);
#endif
    }
    void StdConsole::printLine(char const * const msg, char const * const color) {
#if WIN32
        std::printf("%s%s%s\r\n", color, msg, StdConsoleColor::NO_COLOR);
#else
        std::printf("%s%s%s\n", color, msg, StdConsoleColor::NO_COLOR);
#endif
    }
#endif
}

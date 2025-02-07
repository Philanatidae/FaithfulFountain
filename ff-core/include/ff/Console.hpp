/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_CONSOLE_HPP
#define _FAITHFUL_FOUNTAIN_CONSOLE_HPP

#include <memory>
#include <string>
#include <iostream>
#include <tinyformat/tinyformat.h>
#include <cstdlib>
#include <ff/util/Macros.hpp>
#include <vector>
#include <ff/util/Time.hpp>

namespace ff {
    enum class ConsoleLevel {
        LOG = 0,
        WARN,
        ERROR,

        COUNT
    };

    struct ConsoleLogEntry {
        ConsoleLevel level;
        timestamp_t timestamp;
        std::string message;
    };

    class Console {
    friend class DevToolsCore;
    friend class ClearConsoleDirector;

    public:
        virtual ~Console();

        template <class... Args>
        static inline void log(const std::string& formatString, const Args&... args) {
            log(tfm::format(formatString.c_str(), args...));
        }
        template <class... Args>
        static inline void logWithIdentifier(const std::string& identifier, const std::string& formatString, const Args&... args) {
            logWithIdentifier(identifier, tinyformat::format(formatString.c_str(), args...));
        }
        static inline void logWithIdentifier(const std::string& identifier, const std::string& msg) {
            log(tinyformat::format("[%s]: %s", identifier, msg));
        }
        static inline void log(const std::string& msg) {
            if(getConsoleInstance() != nullptr) {
                getConsoleInstance()->logImpl(msg);
            }
            addLogEntry(ConsoleLevel::LOG, msg);
        }

        template <class... Args>
        static inline void warn(const std::string& formatString, const Args&... args) {
            warn(tfm::format(formatString.c_str(), args...));
        }
        template <class... Args>
        static inline void warnWithIdentifier(const std::string& identifier, const std::string& formatString, const Args&... args) {
            warnWithIdentifier(identifier, tinyformat::format(formatString.c_str(), args...));
        }
        static inline void warnWithIdentifier(const std::string& identifier, const std::string& msg) {
            warn(tinyformat::format("[%s]: %s", identifier, msg));
        }
        static inline void warn(const std::string& msg) {
            if(getConsoleInstance() != nullptr) {
                getConsoleInstance()->warnImpl(msg);
            }
            addLogEntry(ConsoleLevel::WARN, msg);
        }

        template <class... Args>
        static inline void error(const std::string& formatString, const Args&... args) {
            error(tfm::format(formatString.c_str(), args...));
        }
        template <class... Args>
        static inline void errorWithIdentifier(const std::string& identifier, const std::string& formatString, const Args&... args) {
            errorWithIdentifier(identifier, tinyformat::format(formatString.c_str(), args...));
        }
        static inline void errorWithIdentifier(const std::string& identifier, const std::string& msg) {
            error(tinyformat::format("[%s]: %s", identifier, msg));
        }
        static inline void error(const std::string& msg) {
            if(getConsoleInstance() != nullptr) {
                getConsoleInstance()->errorImpl(msg);
            }
            addLogEntry(ConsoleLevel::ERROR, msg);
        }

        static inline void setConsoleInstance(Console* consolePtr) {
            getConsoleInstance() = std::unique_ptr<Console>(consolePtr);
        }
        static inline void setConsoleInstance(std::unique_ptr<Console>&& consolePtr) {
            getConsoleInstance() = std::move(consolePtr);
        }

        static void printStackTrace();

        static void attemptToBreakDebugger();
    protected:
        virtual void logImpl(const std::string& msg) = 0;
        virtual void warnImpl(const std::string& msg) = 0;
        virtual void errorImpl(const std::string& msg) = 0;

    private:
        static std::unique_ptr<Console>& getConsoleInstance();
        static std::vector<ConsoleLogEntry>& getLog();

        static void addLogEntry(ConsoleLevel const& level, std::string const& msg);
    };

    namespace StdConsoleColor {
        constexpr const char* NO_COLOR = "\033[0m";
        constexpr const char* BLACK = "\033[0;30m";
        constexpr const char* RED = "\033[0;31m";
        constexpr const char* GREEN = "\033[0;32m";
        constexpr const char* YELLOW = "\033[0;33m";
        constexpr const char* BLUE = "\033[0;34m";
        constexpr const char* PURPLE = "\033[0;35m";
        constexpr const char* CYAN = "\033[0;36m";
        constexpr const char* LIGHT_GRAY = "\033[0;37m";
        constexpr const char* DARK_GRAY = "\033[1;30m";
        constexpr const char* LIGHT_RED = "\033[1;31m";
        constexpr const char* LIGHT_GREEN = "\033[1;32m";
        constexpr const char* BRIGHT_YELLOW = "\033[1;33m";
        constexpr const char* LIGHT_BLUE = "\033[1;34m";
        constexpr const char* LIGHT_PURPLE = "\033[1;35m";
        constexpr const char* LIGHT_CYAN = "\033[1;36m";
        constexpr const char* WHITE = "\033[1;37m";
    }

    class StdConsole : public Console {
    public:
        virtual ~StdConsole();

    protected:
        void logImpl(const std::string& msg) override;
        void warnImpl(const std::string& msg) override;
        void errorImpl(const std::string& msg) override;

    private:
        void printLine(char const * const msg, char const * const color);
        void printLine(char const * const msg);
    };
}

#define FF_CONSOLE_LOG(...) \
    { \
        std::string _source_path(__FILE__); \
        const size_t lastSlash = _source_path.find_last_of("\\/"); \
        std::string _source_filename(_source_path, lastSlash + 1); \
        const size_t firstDot = _source_filename.find_first_of("."); \
        const std::string _cleanIdentifier(_source_filename, 0, firstDot); \
        ff::Console::logWithIdentifier(tinyformat::format("%s:%s", _cleanIdentifier, __LINE__), __VA_ARGS__); \
    }

#define FF_CONSOLE_ERROR(...) \
    { \
        std::string _source_path(__FILE__); \
        const size_t lastSlash = _source_path.find_last_of("\\/"); \
        std::string _source_filename(_source_path, lastSlash + 1); \
        const size_t firstDot = _source_filename.find_first_of("."); \
        const std::string _cleanIdentifier(_source_filename, 0, firstDot); \
        ff::Console::errorWithIdentifier(tinyformat::format("%s:%s", _cleanIdentifier, __LINE__), __VA_ARGS__); \
    }
#define FF_CONSOLE_WARN(...) \
    { \
        std::string _source_path(__FILE__); \
        const size_t lastSlash = _source_path.find_last_of("\\/"); \
        std::string _source_filename(_source_path, lastSlash + 1); \
        const size_t firstDot = _source_filename.find_first_of("."); \
        const std::string _cleanIdentifier(_source_filename, 0, firstDot); \
        ff::Console::warnWithIdentifier(tinyformat::format("%s:%s", _cleanIdentifier, __LINE__), __VA_ARGS__); \
    }

#define FF_CONSOLE_FATAL(...) FF_CONSOLE_ERROR(__VA_ARGS__); exit(EXIT_FAILURE);

#define FF_ASSERT(expr, ...) if(!(expr)) { FF_CONSOLE_ERROR(__VA_ARGS__); ff::Console::printStackTrace(); ff::Console::attemptToBreakDebugger(); exit(EXIT_FAILURE); }

#endif

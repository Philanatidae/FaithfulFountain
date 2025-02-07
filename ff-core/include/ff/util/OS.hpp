/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_UTIL_OS_HPP
#define _FAITHFUL_FOUNTAIN_UTIL_OS_HPP

#include <string>

#include <ff/util/Time.hpp>
#include <filesystem>

namespace ff {
    inline std::string removeFileExtension(const std::string& filename) {
        size_t lastdot = filename.find_last_of(".");
        if (lastdot == std::string::npos) return filename;
        return filename.substr(0, lastdot); 
    }
    inline std::string getFileExtension(const std::string& filename) {
        size_t lastdot = filename.find_last_of(".");
        if (lastdot == std::string::npos) return "";
        return filename.substr(lastdot + 1, filename.size()); 
    }

    timestamp_t getFileLastModifiedTime(const std::string& filename);

    bool copyFileIfNewer(const std::filesystem::path& source,
        const std::filesystem::path& target,
        const bool& copyIfNotExists = true);
}

#endif

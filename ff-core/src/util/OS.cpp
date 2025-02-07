/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/util/OS.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

namespace ff {
    timestamp_t getFileLastModifiedTime(const std::string& filename) {
        // Source: https://stackoverflow.com/a/40504396
#ifdef WIN32
        struct _stat64 s;
        if (_stat64(filename.c_str(), &s) == 0) {
            return s.st_mtime * 1000;
        }
#else
        struct stat s;
        if (stat(filename.c_str(), &s) == 0) {
            return s.st_mtime * 1000;
        }
#endif
        return -1;
    }

    bool copyFileIfNewer(const std::filesystem::path& source,
        const std::filesystem::path& target,
        const bool& copyIfNotExists) {
        if(std::filesystem::exists(target)) {
            std::filesystem::copy_file(source, target, std::filesystem::copy_options::overwrite_existing);
            return true;
        } else {
            if(!copyIfNotExists) {
                return false;
            }
            std::filesystem::copy_file(source, target, std::filesystem::copy_options::none);
            return true;
        }
    }
}

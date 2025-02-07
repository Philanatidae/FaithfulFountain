/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_UTIL_STRING_HPP
#define _FAITHFUL_FOUNTAIN_UTIL_STRING_HPP

#include <string>
#include <vector>
#include <sstream>

// @todo Add namespace
namespace ff {
    inline std::vector<std::string> split(const std::string& s, char delim) {
        std::istringstream ss(s);
        std::string item;
        std::vector<std::string> elems;
        while(std::getline(ss, item, delim)) {
            if(item.empty()) {
                continue;
            }
            elems.push_back(std::move(item));
        }
        return elems;
    }

    // https://stackoverflow.com/a/44973498/1457856
    inline std::string trim(const std::string& s) {
        std::string sCopy(s);
        sCopy.erase(sCopy.begin(), std::find_if(sCopy.begin(), sCopy.end(), [](int c) {
           return !std::isspace(c); 
        }));
        return sCopy;
    }

    inline std::string toLower(const std::string& s) {
        std::string sCopy(s);
        std::transform(sCopy.begin(), sCopy.end(), sCopy.begin(), ::tolower);
        return sCopy;
    }
    inline std::string toUpper(const std::string& s) {
        std::string sCopy(s);
        std::transform(sCopy.begin(), sCopy.end(), sCopy.begin(), ::toupper);
        return sCopy;
    }

    // @todo Probably need to put this in a source file instead of the header
    inline bool compareCaseInsensitive(const std::string& a, const std::string& b) {
        if(a.size() != b.size()) {
            return false;
        }
        for(size_t i = 0; i < a.size(); i++) {
            char charA = a[i];
            char charB = b[i];
            if(charA != charB) {
                if(charA >= 'A' && charA <= 'Z') {
                    charA += 32;
                }
                if(charB >= 'A' && charB <= 'Z') {
                    charB += 32;
                }
                if(charA != charB) {
                    return false;
                }
            }
        }
        return true;
    }
}

#endif

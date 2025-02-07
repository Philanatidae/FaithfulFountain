/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/io/INIFile.hpp>

#include <sstream>
#include <iomanip>
#include <tinyformat/tinyformat.h>
#include <ff/util/String.hpp>
#include <set>

namespace ff {
    INIFile::INIFile() {
    }
    INIFile::INIFile(BinaryReader& reader) {
        populateFromReader(reader);
    }
    INIFile::INIFile(const INIFile& other)
        :_iniMap(other._iniMap) {
    }
    INIFile::~INIFile() {
    }

    const INIMap& INIFile::getMap() const {
        return _iniMap;
    }
    INIMap& INIFile::getMap() {
        return _iniMap;
    }

    bool INIFile::containsKey(const std::string& key) const {
        return _iniMap.find(key) != _iniMap.end();
    }
    const std::string& INIFile::getRawValue(const std::string& key) const {
        FF_ASSERT(containsKey(key), "INI map does not contain key `%s`.", key);
        return _iniMap.find(key)->second;
    }
    void INIFile::setRawValue(const std::string& key, const std::string& val) {
        if (containsKey(key)) {
            _iniMap.find(key)->second = val;
        }
        else {
            _iniMap.emplace(key, val);
        }
    }

    std::string INIFile::getValueAsString(const std::string& key) const {
        // @todo Might be better in the future to rewrite std::quoted in string utilities
        std::istringstream ss(getRawValue(key));
        std::string value;
        ss >> std::quoted(value);
        return value;
    }
    bool INIFile::getValueAsBool(const std::string& key) const {
        std::string rawVal = getRawValue(key);
        std::transform(rawVal.begin(), rawVal.end(), rawVal.begin(), ::tolower);
        rawVal = trim(rawVal);
        if (rawVal == "y"
            || rawVal == "yes"
            || rawVal == "t"
            || rawVal == "true"
            || rawVal == "1"
            || rawVal == "on") {
            return true;
        }
        else {
            return false;
        }
    }
    
    template<>
    std::string INIFile::getValue<std::string>(const std::string& key) const {
        return getValueAsString(key);
    }
    template<>
    bool INIFile::getValue<bool>(const std::string& key) const {
        return getValueAsBool(key);
    }

    void INIFile::setValueAsString(const std::string& key, const std::string& val) {
        std::ostringstream ss;
        ss << std::quoted(val);
        setRawValue(key, ss.str());
    }
    void INIFile::setValueAsString(const std::string& key, char const* const& val) {
        std::ostringstream ss;
        ss << std::quoted(val);
        setRawValue(key, ss.str());
    }
    void INIFile::setValueAsBool(const std::string& key, const bool& val) {
        setRawValue(key, val ? "1" : "0");
    }

    template<>
    void INIFile::setValue<std::string>(const std::string& key, const std::string& val) {
        setValueAsString(key, val);
    }
    template<>
    void INIFile::setValue<char const* const>(const std::string& key, char const* const& val) {
        setValueAsString(key, val);
    }
    template<>
    void INIFile::setValue<bool>(const std::string& key, const bool& val) {
        setValueAsBool(key, val);
    }

    void INIFile::clear() {
        _iniMap.clear();
    }
    int INIFile::size() const {
        return (int)_iniMap.size();
    }

    std::string INIFile::toString() const {
        std::ostringstream ss;
        // Sort alphabetically first (we store out of order)
        std::set<std::string> namesSorted;
        for(const auto& pair : _iniMap) {
            namesSorted.insert(pair.first);
        }
        // We can loop over the sorted names and write to string
        for (const auto& name : namesSorted) {
            ss << tinyformat::format("%s=%s", name, getRawValue(name)) << std::endl;
        }
        return ss.str();
    }
    bool INIFile::outputToWriter(BinaryWriter& writer) {
        std::string iniString = toString();
        return writer.write((uint8_t*)iniString.data(), (int)iniString.size()) == iniString.size();
    }

    bool INIFile::populateFromString(const std::string& iniString) {
        std::istringstream ss(iniString);
        std::string line;
        int lineNumber = 0;
        INIMap tempMap;
        while (std::getline(ss, line)) {
            lineNumber++;

            if(line.empty()) {
                continue;
            }
            if(line[0] == ';') {
                // Comment; skip line
                // @todo Decide if there can be preceding spaces before comments and verify that it works
                continue;
            }
            std::vector<std::string> vals = split(line, '=');
            if(vals.size() == 1) {
                FF_CONSOLE_ERROR("INI is not in the correct format (expected '=' on line %s).", lineNumber);
                return false;
            }
            if(vals.size() == 0) {
                FF_CONSOLE_ERROR("Unknown error on line %s.", lineNumber);
                return false;
            }
            tempMap.emplace(vals[0], vals[1]); // name, val
        }
        // Parse successful; we can now merge.
        for (const auto& tempPair : tempMap) {
            setRawValue(tempPair.first, tempPair.second);
        }
        return true;
    }
    bool INIFile::populateFromMemory(const BinaryMemory& memory) {
        return populateFromString(memory.toString());
    }
    bool INIFile::populateFromReader(BinaryReader& reader) {
        BinaryMemory memory(reader);
        return populateFromMemory(memory);
    }
}

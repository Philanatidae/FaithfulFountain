/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_IO_INI_FILE_HPP
#define _FAITHFUL_FOUNTAIN_IO_INI_FILE_HPP

#include <string>
#include <unordered_map>

#include <ff/io/BinaryReader.hpp>
#include <ff/io/BinaryMemory.hpp>
#include <ff/io/BinaryWriter.hpp>

#include <iomanip>

namespace ff {
    using INIMap = std::unordered_map<std::string, std::string>;
    constexpr int INI_FILE_FLOAT_PRECISION = 8;

    class INIFile {
    public:
        INIFile();
        INIFile(BinaryReader& reader);
        INIFile(const INIFile& other);
        virtual ~INIFile();

        const INIMap& getMap() const;
        INIMap& getMap();

        bool containsKey(const std::string& key) const;
        const std::string& getRawValue(const std::string& key) const;
        void setRawValue(const std::string& key, const std::string& val);

        std::string getValueAsString(const std::string& key) const;
        bool getValueAsBool(const std::string& key) const;

        template<typename T>
        T getValue(const std::string& key) const;

        void setValueAsString(const std::string& key, const std::string& val);
        void setValueAsString(const std::string& key, char const* const& val);
        void setValueAsBool(const std::string& key, const bool& val);

        template<typename T>
        void setValue(const std::string& key, const T& val);

        void clear();
        int size() const;

        std::string toString() const;
        bool outputToWriter(BinaryWriter& writer);

        bool populateFromString(const std::string& iniString);
        bool populateFromMemory(const BinaryMemory& memory);
        bool populateFromReader(BinaryReader& reader);

    private:
        INIMap _iniMap;
    };

    template<typename T>
    void INIFile::setValue(const std::string& key, const T& val) {
        std::ostringstream ss;
        ss << std::setprecision(INI_FILE_FLOAT_PRECISION);
        ss << val;
        setRawValue(key, ss.str());
    }

    template<>
    void INIFile::setValue<std::string>(const std::string& key, const std::string& val);
    template<>
    void INIFile::setValue<char const* const>(const std::string& key, char const* const& val);
    template<>
    void INIFile::setValue<bool>(const std::string& key, const bool& val);

    template<typename T>
    T INIFile::getValue(const std::string& key) const {
        std::istringstream ss(getRawValue(key));
        T val;
        ss >> std::setprecision(INI_FILE_FLOAT_PRECISION);
        ss >> val;
        return val;
    }

    template<>
    std::string INIFile::getValue<std::string>(const std::string& key) const;
    template<>
    bool INIFile::getValue<bool>(const std::string& key) const;
}

#endif

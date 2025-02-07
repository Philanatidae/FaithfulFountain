/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/env/StdDataStorage.hpp>

#include <ff/io/StreamBinaryReader.hpp>
#include <ff/io/StreamBinaryWriter.hpp>

#include <fstream>

namespace ff {
    StdDataStorage::StdDataStorage(const std::string& configDirectory)
        :_configDirectory(configDirectory) {
    }
    StdDataStorage::~StdDataStorage() {
    }

    std::filesystem::path StdDataStorage::getConfigPath(const std::string& name) const {
        return _configDirectory/name;
    }
    bool StdDataStorage::doesConfigExist(const std::string& name) const {
        return std::filesystem::exists(getConfigPath(name));
    }
    std::shared_ptr<BinaryReader> StdDataStorage::getConfigReader(const std::string& name) const {
        if(!doesConfigExist(name)) {
            return nullptr;
        }
        return std::make_shared<StreamBinaryReader>(std::make_shared<std::ifstream>(getConfigPath(name), std::ios::binary));
    }
    std::shared_ptr<BinaryWriter> StdDataStorage::getConfigWriter(const std::string& name) const {
        if(!std::filesystem::exists(_configDirectory)) {
            std::filesystem::create_directories(_configDirectory);
        }
        return std::make_shared<StreamBinaryWriter>(std::make_shared<std::ofstream>(getConfigPath(name), std::ios::binary));
    }
}

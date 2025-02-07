/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ENV_STD_DATA_STORAGE_HPP
#define _FAITHFUL_FOUNTAIN_ENV_STD_DATA_STORAGE_HPP

#include <ff/env/IDataStorage.hpp>

#include <string>
#include <filesystem>

namespace ff {
    class StdDataStorage : public IDataStorage {
    public:
        StdDataStorage(const std::string& configDirectory);
        virtual ~StdDataStorage();

        std::filesystem::path getConfigPath(const std::string& name) const;
        bool doesConfigExist(const std::string& name) const override;
        std::shared_ptr<BinaryReader> getConfigReader(const std::string& name) const override;
        std::shared_ptr<BinaryWriter> getConfigWriter(const std::string& name) const override;


    private:
        std::filesystem::path _configDirectory;
    };
}

#endif

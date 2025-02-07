/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ENV_IDATA_STORAGE_HPP
#define _FAITHFUL_FOUNTAIN_ENV_IDATA_STORAGE_HPP

#include <ff/io/BinaryReader.hpp>
#include <ff/io/BinaryWriter.hpp>

#include <string>
#include <memory>

namespace ff {
    class IDataStorage {
    public:
        IDataStorage() { }
        virtual ~IDataStorage() { }

        virtual bool doesConfigExist(const std::string& name) const = 0;
        virtual std::shared_ptr<BinaryReader> getConfigReader(const std::string& name) const = 0;
        virtual std::shared_ptr<BinaryWriter> getConfigWriter(const std::string& name) const = 0;
    };

    class NullDataStorage : public IDataStorage {
    public:
        NullDataStorage() { }
        virtual ~NullDataStorage() { }

        bool doesConfigExist(const std::string& name) const override {
            return false;
        }
        std::shared_ptr<BinaryReader> getConfigReader(const std::string& name) const override {
            return nullptr;
        }
        std::shared_ptr<BinaryWriter> getConfigWriter(const std::string& name) const override {
            return nullptr;
        }
    };
}

#endif

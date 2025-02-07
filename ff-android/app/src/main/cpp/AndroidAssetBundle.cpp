/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "AndroidAssetBundle.hpp"

#include <memory>

#include <ff/Console.hpp>

#include "AndroidBinaryReader.hpp"
#include <ff/io/BinaryMemory.hpp>

namespace ff {
    AndroidAssetBundle::AndroidAssetBundle(AAssetManager* const& assetManager)
        :_assetManager(assetManager) {
    }
    AndroidAssetBundle::~AndroidAssetBundle() {
    }

    std::shared_ptr<BinaryReader> AndroidAssetBundle::getAssetReader(const std::string& path) {
        AAsset* asset = AAssetManager_open(_assetManager, path.c_str(), AASSET_MODE_BUFFER);
        FF_ASSERT(asset, "Asset `%s` does not exist in bundle.", path);

        auto readerPtr = std::make_shared<AndroidBinaryReader>(asset);
        return readerPtr;
    }

    AAssetManager* AndroidAssetBundle::getAssetManager() const {
        return _assetManager;
    }

    void AndroidAssetBundle::onInit() {
    }
    nlohmann::json AndroidAssetBundle::loadIndexObject() {
        AAsset* indexAsset = AAssetManager_open(_assetManager, "INDEX", AASSET_MODE_BUFFER);
        FF_ASSERT(indexAsset, "INDEX does not exist in bundle.");

        AndroidBinaryReader reader(indexAsset);
        BinaryMemory memory(reader);
        return nlohmann::json::parse(memory.toString());
    }
}

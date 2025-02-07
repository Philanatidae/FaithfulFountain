/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/assets/DirectoryAssetBundle.hpp>

#include <filesystem>
#include <fstream>

#include <ff/Console.hpp>
#include <ff/CVars.hpp>
#include <ff/util/OS.hpp>
#include <tinyformat/tinyformat.h>

#include <ff/io/StreamBinaryReader.hpp>
#include <ff/io/BinaryMemory.hpp>

namespace ff {
    DirectoryAssetBundle::DirectoryAssetBundle() {
    }
    DirectoryAssetBundle::~DirectoryAssetBundle() {
    }

    std::shared_ptr<BinaryReader> DirectoryAssetBundle::getAssetReader(const std::string& path) {
        auto assetPath = std::filesystem::relative(std::filesystem::path(CVars::get<std::string>("asset_bundle_path"))/std::filesystem::path(path));
        FF_ASSERT(std::filesystem::exists(assetPath), "Path `%s` does not exist in bundle.", assetPath);

        auto readerPtr = std::make_shared<StreamBinaryReader>(std::make_shared<std::ifstream>(assetPath.string(), std::ios::binary));
        return readerPtr;
    }

    void DirectoryAssetBundle::onInit() {

    }
    nlohmann::json DirectoryAssetBundle::loadIndexObject() {
        BinaryMemory indexObjectMemory(*getAssetReader("INDEX"));
        return nlohmann::json::parse(indexObjectMemory.toString());
    }
}

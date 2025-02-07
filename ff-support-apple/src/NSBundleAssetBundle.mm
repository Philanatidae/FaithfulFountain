/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-support-apple/NSBundleAssetBundle.hmm>

#include <filesystem>
#include <fstream>

#include <ff/io/StreamBinaryReader.hpp>
#include <ff/io/BinaryMemory.hpp>

namespace ff {
    NSBundleAssetBundle::NSBundleAssetBundle(NSBundle* bundle, NSString* directory)
        :_bundle(bundle),_directory(directory) {
    }
    NSBundleAssetBundle::~NSBundleAssetBundle() {
    }

    std::shared_ptr<BinaryReader> NSBundleAssetBundle::getAssetReader(const std::string& path) {
        NSString* assetPath = nil;
        if(_directory == nil) {
            assetPath = [_bundle pathForResource:[NSString stringWithUTF8String:path.c_str()] ofType:@""];
        } else {
            assetPath = [_bundle pathForResource:[NSString stringWithUTF8String:path.c_str()] ofType:@"" inDirectory:_directory];
        }
        FF_ASSERT(assetPath != nil, "Path `%s` does not exist in bundle.", path);

        return std::make_shared<StreamBinaryReader>(std::make_shared<std::ifstream>([assetPath UTF8String], std::ios::binary));
    }

    void NSBundleAssetBundle::onInit() {
    }
    nlohmann::json NSBundleAssetBundle::loadIndexObject() {
        BinaryMemory indexObjectMem(*getAssetReader("INDEX"));
        return nlohmann::json::parse(indexObjectMem.toString());
    }
}

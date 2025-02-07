/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/assets/IAssetBundle.hpp>

namespace ff {
    IAssetBundle::IAssetBundle() {
    }
    IAssetBundle::~IAssetBundle() {
    }

    void IAssetBundle::init() {
        onInit();

        detectAssets();
    }

    void IAssetBundle::releaseCachedContent() {
        for(auto p : _cache) {
            delete p.second;
        }
        _cache.clear();
    }

    nlohmann::json IAssetBundle::getBundleIndexObject() const {
        return _indexObject;
    }
    bool IAssetBundle::isValidAsset(const std::string& name) const {
        return _indexObject.find(name) != _indexObject.end();
    }
    nlohmann::json IAssetBundle::getAssetObject(const std::string& name) const {
        auto it = _indexObject.find(name);
        FF_ASSERT(it != _indexObject.end(), "Invalid asset name (%s).", name);
        FF_ASSERT(it.value().is_object(), "Asset object (`%s`) is not an object.", name);
        return it.value();
    }

    void IAssetBundle::detectAssets() {
        _indexObject = loadIndexObject();

        FF_ASSERT(_indexObject["version"] == 0, "Bundle version must be 0.");
#if defined(FF_IS_RELEASE)
        FF_ASSERT(_indexObject["valid"] == true, "Bundle must be valid.");
#else
        if(_indexObject["valid"] == false) {
            FF_CONSOLE_ERROR("Asset bundle is not valid for distribution; use this bundle ONLY for local development.");
        }
#endif

        for(auto it = _indexObject.begin(); it != _indexObject.end(); ++it) {
            if(it.key() == "version"
                || it.key() == "valid"
                || it.key() == "platform") {
                continue;
            }
            auto obj = it.value();
            FF_ASSERT(obj.is_object(), "Unknown key in INDEX: `%s`.", it.key());

            FF_ASSERT(obj.find("name") != obj.end(), "Missing `name` key for asset `%s` in INDEX.", it.key());
            FF_ASSERT(obj.find("type") != obj.end(), "Missing `type` key for asset `%s` in INDEX.", it.key());
            FF_CONSOLE_LOG("Detected asset: `%s` (`%s`)", obj["name"], obj["type"]);
        }
    }

    NullAssetBundle::NullAssetBundle() {
    }
    NullAssetBundle::~NullAssetBundle() {
    }

    std::shared_ptr<BinaryReader> NullAssetBundle::getAssetReader(const std::string& path) {
        return nullptr;
    }

    void NullAssetBundle::onInit() {
    }
    nlohmann::json NullAssetBundle::loadIndexObject() {
        return nlohmann::json::object();
    }
}

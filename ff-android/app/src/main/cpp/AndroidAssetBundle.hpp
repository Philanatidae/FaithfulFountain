/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ANDROID_ANDROID_ASSET_BUNDLE_HPP
#define _FAITHFUL_FOUNTAIN_ANDROID_ANDROID_ASSET_BUNDLE_HPP

#include <ff/assets/IAssetBundle.hpp>
#include <unordered_map>
#include <string>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

namespace ff {
    class AndroidAssetBundle : public IAssetBundle {
    public:
        AndroidAssetBundle(AAssetManager* const& assetManager);
        virtual ~AndroidAssetBundle();

        virtual std::shared_ptr<BinaryReader> getAssetReader(const std::string& path) override;

        AAssetManager* getAssetManager() const;

    protected:
        AAssetManager* _assetManager;

        virtual void onInit() override;
        virtual nlohmann::json loadIndexObject() override;
    };
}

#endif

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSETS_DIRECTORY_ASSET_BUNDLE_HPP
#define _FAITHFUL_FOUNTAIN_ASSETS_DIRECTORY_ASSET_BUNDLE_HPP

#include <ff/assets/IAssetBundle.hpp>

namespace ff {
    class DirectoryAssetBundle : public IAssetBundle {
    public:
        DirectoryAssetBundle();
        virtual ~DirectoryAssetBundle();

        virtual std::shared_ptr<BinaryReader> getAssetReader(const std::string& path);

    protected:
        virtual void onInit();
        virtual nlohmann::json loadIndexObject();
    };
}

#endif

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "AndroidBinaryReader.hpp"

#include <glm/glm.hpp>

namespace ff {
    AndroidBinaryReader::AndroidBinaryReader(AAsset* const& asset)
        :_asset(asset, [](AAsset* ptr) {
            if(ptr != nullptr) {
                AAsset_close(ptr);
            }
        }) {
    }
    AndroidBinaryReader::~AndroidBinaryReader() {
    }

    int AndroidBinaryReader::getSize() const {
        return AAsset_getLength(getAsset());
    }
    int AndroidBinaryReader::read(uint8_t* const& ptr, const int& count, const int& offset) {
        //AAsset_seek(getAsset(), offset, 0);
        int bytesToRead = glm::min(count, getSize() - offset);
        int len = AAsset_getLength(getAsset());
        return AAsset_read(getAsset(), reinterpret_cast<char*>(ptr), bytesToRead);
    }
    AAsset* AndroidBinaryReader::getAsset() const {
        return _asset.get();
    }
}

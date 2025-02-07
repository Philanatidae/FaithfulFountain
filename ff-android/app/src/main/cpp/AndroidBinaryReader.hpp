/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ANDROID_BINARY_READER_HPP
#define _FAITHFUL_FOUNTAIN_ANDROID_BINARY_READER_HPP

#include <ff/io/BinaryReader.hpp>
#include <memory>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

namespace ff {
    class AndroidBinaryReader : public BinaryReader {
    public:
        AndroidBinaryReader(AAsset* const& asset);
        virtual ~AndroidBinaryReader();

        int getSize() const;
        int read(uint8_t* const& ptr, const int& count, const int& offset = 0);
        AAsset* getAsset() const;

    private:
        std::unique_ptr<AAsset, void(*)(AAsset*)> _asset;
    };
}

#endif

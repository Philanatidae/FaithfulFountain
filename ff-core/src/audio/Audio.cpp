/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/audio/Audio.hpp>

#include <ff/audio/OggAudioSource.hpp>

#include <ff/io/BinaryMemory.hpp>

namespace ff {
    Audio::Audio(IAssetBundle& assetBundle, const nlohmann::json& assetObject)
        :_ogg(nullptr) {
        FF_ASSET_TYPE_CHECK(assetObject, "Audio");

        FF_ASSERT(!assetObject["path"].is_null(), "Missing `path` in asset object.");

        auto binaryMemory = assetBundle.load<ff::BinaryMemory>(assetObject["path"]);

        {
            int error;
            _ogg = stb_vorbis_open_memory(binaryMemory->data(), (int)binaryMemory->size(), &error, nullptr);
            FF_ASSERT(_ogg, "OGG file could not be opened.");
            _info = stb_vorbis_get_info(_ogg);

            FF_CONSOLE_LOG("Loaded audio `%s`: [channels: %s, sample rate: %s]", assetObject["name"], _info.channels, _info.sample_rate);
        }
    }
    Audio::~Audio() {
        if(_ogg) {
            stb_vorbis_close(_ogg);
        }
    }

    stb_vorbis* const& Audio::getOgg() const {
        return _ogg;
    }

    int Audio::getSampleRate() const {
        return _info.sample_rate;
    }
    int Audio::getChannelCount() const {
        return _info.channels;
    }
}

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_AUDIO_AUDIO_HPP
#define _FAITHFUL_FOUNTAIN_AUDIO_AUDIO_HPP

#include <ff/audio/IAudioSourceProvider.hpp>
#include <stb_vorbis.h>

#include <string>

#include <ff/Console.hpp>

#include <ff/assets/IAssetBundle.hpp>

namespace ff {
    struct Audio {
        Audio(IAssetBundle& assetBundle, const nlohmann::json& assetObject);
        ~Audio();

        stb_vorbis* const& getOgg() const;

        int getSampleRate() const;
        int getChannelCount() const;
    private:
        stb_vorbis* _ogg;
        stb_vorbis_info _info;
    };
}

#endif

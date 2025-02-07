/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_AUDIO_IAUDIO_SOURCE_PROVIDER_HPP
#define _FAITHFUL_FOUNTAIN_AUDIO_IAUDIO_SOURCE_PROVIDER_HPP

#include <ff/audio/AudioSource.hpp>
#include <memory>

namespace ff {
    struct IAudioSourceProvider {
        virtual ~IAudioSourceProvider() {}

        virtual std::shared_ptr<AudioSource> getAudioSource() = 0;
    };
}

#endif

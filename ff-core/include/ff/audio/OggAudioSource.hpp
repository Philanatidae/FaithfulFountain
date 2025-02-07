/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_AUDIO_OGG_AUDIO_SOURCE_HPP
#define _FAITHFUL_FOUNTAIN_AUDIO_OGG_AUDIO_SOURCE_HPP

#include <ff/audio/AudioSource.hpp>
#include <stb_vorbis.h>
#include <memory>
#include <ff/audio/Audio.hpp>
#include <array>
#include <glm/glm.hpp>
#include <ff/resources/ResourceHandle.hpp>

namespace ff {
    struct Audio;

    class OggAudioSource : public AudioSource {
    public:
        OggAudioSource(const ResourceHandle<Audio>& oggAudio);
        ~OggAudioSource();

        AudioSourceStatus getStatus() const override;

        int fillBufferInterleaved(const int& channels, float* buffer, const int& frames, const int& sampleRate) override;

    private:
        ResourceHandle<Audio> _oggAudio;
        int _framesPlayed;
        AudioSourceStatus _status;
        std::array<float, 4096> _localBuffer; // AUDIO_CORE_CALLBACK_WORKING_BUFFER, which isn't accessible here
        float _remainder;

        float smootherstep(float x) {
            x = glm::clamp(x, 0.0f, 1.0f);
            return x * x * x * (x * (x * 6 - 15) + 10);
        }
    };
}

#endif

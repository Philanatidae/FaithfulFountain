/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_AUDIO_AUDIO_SOURCE_HPP
#define _FAITHFUL_FOUNTAIN_AUDIO_AUDIO_SOURCE_HPP

namespace ff {
    enum class AudioSourceStatus {
        ACTIVE,
        INACTIVE
    };

    class AudioSource {
    public:
        AudioSource() {}
        virtual ~AudioSource() {}

        virtual AudioSourceStatus getStatus() const = 0;

        virtual int fillBufferInterleaved(const int& channels, float* buffer, const int& frames, const int& sampleRate) = 0;
    };
}

#endif

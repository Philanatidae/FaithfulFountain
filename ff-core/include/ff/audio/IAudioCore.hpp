/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_AUDIO_IAUDIO_CORE_HPP
#define _FF_AUDIO_IAUDIO_CORE_HPP

namespace ff {

struct IAudioCore {
    virtual ~IAudioCore() = default;

    
    virtual void bufferFrames(float* const& buffer, const unsigned long& samplesPerChannel, const int& sampleRate) = 0;
};

}

#endif


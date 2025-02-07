/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/audio/OggAudioSource.hpp>

#include <glm/glm.hpp>

namespace ff {
    OggAudioSource::OggAudioSource(const ResourceHandle<Audio>& oggAudio)
        :_oggAudio(oggAudio),_framesPlayed(0),_status(AudioSourceStatus::ACTIVE),
        _remainder(0) {
    }
    OggAudioSource::~OggAudioSource() {
    }
    
    AudioSourceStatus OggAudioSource::getStatus() const {
        return _status;
    }

    int OggAudioSource::fillBufferInterleaved(const int& channels, float* buffer, const int& frames, const int& sampleRate) {
        stb_vorbis_seek(_oggAudio->getOgg(), _framesPlayed);
        if(sampleRate == _oggAudio->getSampleRate()) {
            int framesPerChannel = stb_vorbis_get_samples_float_interleaved(_oggAudio->getOgg(), channels, buffer, frames * channels);
            if(framesPerChannel != frames) {
                _status = AudioSourceStatus::INACTIVE;
            }
            _framesPlayed += framesPerChannel;
            // @todo Remove hardcode. Most likely add a filter system to support per-channel mixing.
            if(channels == 2 && _oggAudio->getChannelCount() == 1) {
                for(int i = 0; i < frames * channels; i++) {
                    if(i % channels == 0) {
                        continue;
                    }
                    buffer[i] = buffer[i - i % channels];
                }
            }
            return framesPerChannel;
        } else {
            std::memset(&_localBuffer[0], 0, sizeof(float) * 4096);

            const float audioSecondsPerSample = 1 / (float)_oggAudio->getSampleRate();
            const float coreSecondsPerSample = 1 / (float)sampleRate;
            
            const float requestedSecondsOfSource = frames * coreSecondsPerSample;
            const float audioSamplesNeeded = (requestedSecondsOfSource + _remainder) / audioSecondsPerSample;
            int framesPerChannel = stb_vorbis_get_samples_float_interleaved(_oggAudio->getOgg(), channels, &_localBuffer[0], (int)glm::ceil(audioSamplesNeeded) * 2);
            
            if(framesPerChannel < (int)glm::ceil(audioSamplesNeeded)) {
                _status = AudioSourceStatus::INACTIVE;
            }

            int mPlayed = 0;
            float elapsedSampleTime = _remainder;
            for(int i = 0; i < frames; i++) {
                int minSample = (int)glm::floor(elapsedSampleTime / audioSecondsPerSample);
                const float mod = glm::mod(elapsedSampleTime, audioSecondsPerSample);
                const float alpha = smootherstep(mod / audioSecondsPerSample);
                float output = glm::mix(_localBuffer[minSample * 2], _localBuffer[(minSample + 1) * 2], alpha);
                buffer[i * 2] = output;

                float output2 = glm::mix(_localBuffer[minSample * 2 + 1], _localBuffer[(minSample + 1) * 2 + 1], alpha);
                buffer[i * 2 + 1] = output2;

                elapsedSampleTime += coreSecondsPerSample;
                mPlayed++;
            }

            _remainder = glm::mod(elapsedSampleTime, audioSecondsPerSample);

            _framesPlayed += framesPerChannel - 1; // We use one more than usual for interpolation

            // @todo Remove hardcode. Most likely add a filter system to support per-channel mixing.
            if(channels == 2 && _oggAudio->getChannelCount() == 1) {
                for(int i = 0; i < frames * channels; i++) {
                    if(i % channels == 0) {
                        continue;
                    }
                    buffer[i] = buffer[i - i % channels];
                }
            }

            return mPlayed;
        }
    }
}

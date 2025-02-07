/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ff/audio/AudioCore.hpp"
#include <ff/audio/IAudioBackend.hpp>

namespace ff {
    IAudioBackend::IAudioBackend() {
    }
    IAudioBackend::~IAudioBackend() {
    }

    IAudioCore* IAudioBackend::getAudioCore() const {
        return _audioCore;
    }

    void IAudioBackend::initialize(IAudioCore* const& audioCore, const int& channels, const int& requestedSampleRate) {
        _audioCore = audioCore;
        onInitialize(channels, requestedSampleRate);
    }
    void IAudioBackend::update(const float& dt) {
        onUpdate(dt);
    }
    void IAudioBackend::kill() {
        onKill();
    }

    void IAudioBackend::onUpdate(const float& dt) {
    }
    void IAudioBackend::onKill() {
    }

    NullAudioBackend::NullAudioBackend() {
    }
    NullAudioBackend::~NullAudioBackend() {
    }

    int NullAudioBackend::getSampleRate() const {
        return _sampleRate;
    }

    void NullAudioBackend::onInitialize(const int& channels, const int& requestedSampleRate) {
        if(requestedSampleRate < 0) {
            _sampleRate = 48000;
        } else {
            _sampleRate = requestedSampleRate;
        }
    }
    void NullAudioBackend:: onUpdate(const float& dt) {
        _acculmulator += dt;
        float timePerBuffer = ((float)_mockBuffer.size() / AUDIO_CORE_CHANNELS) / (float)getSampleRate();
        while(_acculmulator > timePerBuffer) {
            getAudioCore()->bufferFrames(&_mockBuffer[0], _mockBuffer.size() / AUDIO_CORE_CHANNELS, getSampleRate());

            _acculmulator -= timePerBuffer;
        }
    }
    void NullAudioBackend::onRequestPause() {
    }
    void NullAudioBackend::onRequestResume() {
    }
}

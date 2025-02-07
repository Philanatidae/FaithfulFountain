/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/audio/AudioProcess.hpp>

#include <ff/messages/MessageBus.hpp>
#include <ff/Locator.hpp>

#include <ff/audio/OggAudioSource.hpp>

#include <ff/commands/audio/PlayAudioSourceCmd.hpp>
#include <ff/commands/audio/StopAudioSourceCmd.hpp>

namespace ff {
    AudioProcess::AudioProcess(const ResourceHandle<Audio>& audio)
        :_audio(audio),_audioSource(nullptr) {
    }
    AudioProcess::~AudioProcess() {
    }

    bool AudioProcess::processEvent(const AudioSourceInactiveEvent& message) {
        if(message.audioSource == _audioSource) {
            kill();
        }

        return false;
    }

    void AudioProcess::onInitialize() {
        Locator::getMessageBus().addListener<AudioSourceInactiveEvent>(this);

        _audioSource = std::make_shared<OggAudioSource>(_audio);
        Locator::getMessageBus().dispatch<PlayAudioSourceCmd>(_audioSource);
    }
    void AudioProcess::onKill() {
        Locator::getMessageBus().dispatch<StopAudioSourceCmd>(_audioSource);
    }
    void AudioProcess::onTogglePause() {
        if(getPaused()) {
            Locator::getMessageBus().dispatch<StopAudioSourceCmd>(_audioSource);
        } else {
            Locator::getMessageBus().dispatch<PlayAudioSourceCmd>(_audioSource);
        }
    }
}

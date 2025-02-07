/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/audio/AudioCore.hpp>

#include <ff/util/Macros.hpp>
#include <ff/Console.hpp>
#include <ff/CVars.hpp>
#include <ff/Locator.hpp>

#include <cstring>

#include <glm/glm.hpp>

#include <ff/audio/Audio.hpp>

#include <ff/events/audio/AudioSourceInactiveEvent.hpp>

namespace ff {
    AudioCore::AudioCore() {
    }
    AudioCore::~AudioCore() {
    }

    std::unique_ptr<typename PlayAudioSourceCmd::Ret> AudioCore::handleCmd(PlayAudioSourceCmd const& cmd) {
        _sourcesMutex.lock();
        if(std::find(_sources.begin(), _sources.end(), cmd.audioSource) == _sources.end()) {
            _sources.push_back(cmd.audioSource);
        }
        _sourcesMutex.unlock();

        return std::make_unique<typename PlayAudioSourceCmd::Ret>();
    }
    std::unique_ptr<typename StopAudioSourceCmd::Ret> AudioCore::handleCmd(StopAudioSourceCmd const& cmd) {
        _sourcesMutex.lock();
        auto it = std::find(_sources.begin(), _sources.end(), cmd.audioSource);
        if(it != _sources.end()) {
            _sources.erase(it);
        }
        _sourcesMutex.unlock();

        return std::make_unique<typename StopAudioSourceCmd::Ret>();
    }

    bool AudioCore::processEvent(EnvPrepareForSuspendEvent const& evt) {
        Locator::getAudioBackend().onRequestPause();

        return false;
    }
    bool AudioCore::processEvent(EnvPrepareForRestoreEvent const& evt) {
        Locator::getAudioBackend().onRequestResume();

        return false;
    }

    void AudioCore::bufferFrames(float* const& buffer, const unsigned long& samplesPerChannel, const int& sampleRate) {
        // Zero out buffer, we will sum the audio sources
        std::memset(buffer, 0, sizeof(float) * samplesPerChannel * AUDIO_CORE_CHANNELS); // @todo Should channel count not be a constant? Grab from backend?

        _sourcesMutex.lock();
        // @todo Not ideal vvvvvv
        std::vector<std::shared_ptr<AudioSource>> sources = _sources; // Make copy so we can release the unlock the mutex ASAP (not actually ideal :-/)
        _sourcesMutex.unlock();
        if(sources.size() > 0) {
            for(auto it = sources.begin();
                it != sources.end();
                it++) {
                std::shared_ptr<AudioSource> source = *it;

                int start = 0;
                unsigned long sourceFrames = 0;
                while(sourceFrames < samplesPerChannel) {
                    int framesFilled = source->fillBufferInterleaved(AUDIO_CORE_CHANNELS,
                        &_workingBuffer[0],
                        glm::min<int>((int)(samplesPerChannel - sourceFrames), AUDIO_CORE_CALLBACK_WORKING_BUFFER / AUDIO_CORE_CHANNELS),
                        sampleRate); // @todo Xcode is complaining that the glm::min func was casting unsigned long to int implicitely, cast wsas added however I dont iknow if we should be doing this conversion. I don't want to worry about this for now, it's highly unlikely that it will stop working
                    sourceFrames += framesFilled;
                    
                    for(int i = 0; i < framesFilled * AUDIO_CORE_CHANNELS; i++) {
                        buffer[start + i] += _workingBuffer[i];
                    }
                    
                    start += framesFilled * AUDIO_CORE_CHANNELS;

                    if(framesFilled == 0) {
                        break;
                    }
                }
            }
        }

        for(int i = 0; i < samplesPerChannel * AUDIO_CORE_CHANNELS; i++) {
            buffer[i] *= ff::CVars::get<float>("audio_master_volume");
        }
    }

    void AudioCore::onInitialize() {
        Locator::getMessageBus().addHandler<PlayAudioSourceCmd>(this);
        Locator::getMessageBus().addHandler<StopAudioSourceCmd>(this);
        //Locator::getMessageBus().addListener<EnvPrepareForSuspendCommand>(this);
        //Locator::getMessageBus().addListener<EnvPrepareForRestoreCommand>(this);
        FF_CONSOLE_LOG("Audio core initialized.");

        ff::Locator::getAudioBackend().initialize(this, AUDIO_CORE_CHANNELS, -1);
        FF_CONSOLE_LOG("Audio backend initialized.");
    }
    void AudioCore::onUpdate(const float& dt) {
        _sourcesMutex.lock();
        for(size_t i = 0; i < _sources.size(); i++) {
            if(_sources[i] == nullptr) {
                auto it = _sources.begin() + i--;
                _sources.erase(it);
            }
            if(_sources[i]->getStatus() == AudioSourceStatus::INACTIVE) {
                auto it = _sources.begin() + i--;
                Locator::getMessageBus().dispatch<AudioSourceInactiveEvent>(*it);
                _sources.erase(it);
            }
        }
        _sourcesMutex.unlock();

        ff::Locator::getAudioBackend().update(dt);
    }
    void AudioCore::onKill() {
        ff::Locator::getAudioBackend().kill();
    }
}

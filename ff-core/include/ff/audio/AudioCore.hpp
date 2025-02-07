/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_AUDIO_AUDIO_CORE_HPP
#define _FAITHFUL_FOUNTAIN_AUDIO_AUDIO_CORE_HPP

#include <ff/audio/IAudioCore.hpp>

#include <ff/processes/Process.hpp>

#include <memory>
#include <vector>
#include <array>
#include <mutex>

#include <ff/audio/AudioSource.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/messages/CmdHandler.hpp>
#include <ff/commands/audio/PlayAudioSourceCmd.hpp>
#include <ff/commands/audio/StopAudioSourceCmd.hpp>

#include <ff/events/env/EnvPrepareForRestoreEvent.hpp>
#include <ff/events/env/EnvPrepareForSuspendEvent.hpp>

namespace ff {
    constexpr size_t AUDIO_CORE_CALLBACK_WORKING_BUFFER = 4096;
    constexpr int AUDIO_CORE_CHANNELS = 2;

    class AudioCore : public IAudioCore,
        public Process,
        public CmdHandler<PlayAudioSourceCmd>,
        public CmdHandler<StopAudioSourceCmd>,
        public EventListener<EnvPrepareForSuspendEvent>,
        public EventListener<EnvPrepareForRestoreEvent> {
    public:
        AudioCore();
        virtual ~AudioCore();

        void bufferFrames(float* const& buffer, const unsigned long& samplesPerChannel, const int& sampleRate) override;

        std::unique_ptr<typename PlayAudioSourceCmd::Ret> handleCmd(PlayAudioSourceCmd const& cmd) override;
        std::unique_ptr<typename StopAudioSourceCmd::Ret> handleCmd(StopAudioSourceCmd const& cmd) override;

        bool processEvent(const EnvPrepareForSuspendEvent& evt) override;
        bool processEvent(const EnvPrepareForRestoreEvent& evt) override;
    public:
        
        void onInitialize() override;
        void onUpdate(const float& dt) override;
        void onKill() override;

    private:
        std::vector<std::shared_ptr<AudioSource>> _sources;
        std::mutex _sourcesMutex;
        std::array<float, AUDIO_CORE_CALLBACK_WORKING_BUFFER> _workingBuffer;
    };
}

#endif

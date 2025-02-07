/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_AUDIO_AUDIO_PROCESS_HPP
#define _FAITHFUL_FOUNTAIN_AUDIO_AUDIO_PROCESS_HPP

#include <ff/processes/Process.hpp>

#include <ff/resources/ResourceHandle.hpp>
#include <ff/audio/Audio.hpp>
#include <ff/audio/AudioSource.hpp>

#include <memory>

#include <ff/messages/EventListener.hpp>
#include <ff/events/audio/AudioSourceInactiveEvent.hpp>

namespace ff {
    class AudioProcess : public Process,
        EventListener<AudioSourceInactiveEvent> {
    public:
        AudioProcess(const ResourceHandle<Audio>& audio);
        virtual ~AudioProcess();

    protected:
        bool processEvent(const AudioSourceInactiveEvent& message) override;

        void onInitialize() override;
        void onKill() override;
        void onTogglePause() override;

    private:
        ResourceHandle<Audio> _audio;
        std::shared_ptr<AudioSource> _audioSource;
    };
}

#endif

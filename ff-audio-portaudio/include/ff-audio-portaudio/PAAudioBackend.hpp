/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_AUDIO_PA_AUDIO_BACKEND_HPP
#define _FAITHFUL_FOUNTAIN_AUDIO_PA_AUDIO_BACKEND_HPP

#include <ff/audio/IAudioBackend.hpp>

#include <portaudio.h>
#include <memory>

namespace ff {
    class PAAudioBackend final : public IAudioBackend {
    public:
        PAAudioBackend();
        ~PAAudioBackend();
        
        int getSampleRate() const override;

    protected:
        void onInitialize(const int& channels, const int& requestedSampleRate) override;
        void onUpdate(const float& dt) override;
        void onKill() override;
        void onRequestPause() override;
        void onRequestResume() override;

    private:
        std::unique_ptr<PaStream, void(*)(PaStream*)> _stream;
        bool _running;

        static int portaudioCallback(const void* inputBuffer,
            void* outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void* userData);
    };
}

#endif

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_AUDIO_OBOE_AUDIO_BACKEND_HPP
#define _FAITHFUL_FOUNTAIN_AUDIO_OBOE_AUDIO_BACKEND_HPP

#include <ff/audio/IAudioBackend.hpp>

#include <memory>
#include <oboe/Oboe.h>

namespace ff {
    class OboeAudioBackend final : public IAudioBackend,
        public oboe::AudioStreamDataCallback {
    public:
        OboeAudioBackend();
        ~OboeAudioBackend();
        
        int getSampleRate() const override;

        oboe::DataCallbackResult onAudioReady(oboe::AudioStream* audioStream,
                void* audioData,
                int32_t numFrames) override;

    protected:
        void onInitialize(const int& channels, const int& requestedSampleRate) override;
        void onUpdate(const float& dt) override;
        void onKill() override;
        void onRequestPause() override;
        void onRequestResume() override;

    private:
        std::shared_ptr<oboe::AudioStream> _stream;
        oboe::AudioStreamBuilder builder;

        void openStream();
        void closeStream();
    };
}

#endif

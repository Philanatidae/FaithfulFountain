/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_AUDIO_IAUDIO_BACKEND_HPP
#define _FAITHFUL_FOUNTAIN_AUDIO_IAUDIO_BACKEND_HPP

#include <ff/processes/ProcessManager.hpp>
#include <array>
#include <ff/audio/IAudioCore.hpp>

namespace ff {
    class IAudioBackend {
    friend class AudioCore;

    public:
        IAudioBackend();
        virtual ~IAudioBackend();

        IAudioCore* getAudioCore() const;

        virtual int getSampleRate() const = 0;

    protected:
        virtual void onInitialize(const int& channels, const int& requestedSampleRate) = 0;
        virtual void onUpdate(const float& dt);
        virtual void onKill();
        virtual void onRequestPause() = 0;
        virtual void onRequestResume() = 0;

    private:
        IAudioCore* _audioCore;

        void initialize(IAudioCore* const& audioCore, const int& channels, const int& requestedSampleRate);
        void update(const float& dt);
        void kill();
    };

    class NullAudioBackend : public IAudioBackend {
    public:
        NullAudioBackend();
        ~NullAudioBackend();

        int getSampleRate() const override;

    protected:
        void onInitialize(const int& channels, const int& requestedSampleRate) override;
        void onUpdate(const float& dt) override;
        void onRequestPause() override;
        void onRequestResume() override;

    private:
        int _sampleRate;

        float _acculmulator;
        std::array<float, 4096> _mockBuffer;
    };
}

#endif

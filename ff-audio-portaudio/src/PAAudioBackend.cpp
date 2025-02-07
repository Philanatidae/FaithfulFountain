/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ff/Console.hpp"
#include "ff/audio/AudioCore.hpp"
#include "portaudio.h"
#include <ff-audio-portaudio/PAAudioBackend.hpp>
#include <memory>

namespace ff {
    PAAudioBackend::PAAudioBackend()
        :_stream(nullptr, [](PaStream*) {}),
        _running(false) {
    }
    PAAudioBackend::~PAAudioBackend() {
        _stream.reset();
        Pa_Terminate();
    }
    
    int PAAudioBackend::getSampleRate() const {
        auto streamInfo = Pa_GetStreamInfo(_stream.get());
        return (int)streamInfo->sampleRate;
    }

    void PAAudioBackend::onInitialize(const int& channels, const int& requestedSampleRate) {
        auto err = Pa_Initialize();
        FF_ASSERT(err == paNoError, "PortAudio failed to initialize: %s", Pa_GetErrorText(err));

        PaHostApiIndex hostIndex = -1;
        for(PaHostApiIndex i = 0; i < Pa_GetHostApiCount(); i++) {
            auto info = Pa_GetHostApiInfo(i);
            FF_CONSOLE_LOG("Host API found: %s.", info->name);

#ifdef WIN32
            if(std::strcmp(info->name, "Windows WASAPI") == 0) {
                // On Windows, there are three main drivers; wmmv, WASAPI, and WDM/KS.
                // WDM/KS is low-latency but is a direct tie into the driver; sound
                // mixer does not work. WMMV is the most flexible but has very high
                // latency. A good middle-ground is WASAPI. Downside of WASAPI is that
                // we must use its sampling rate. This is not a problem since AudioCore
                // is able to approximate different sample rates via interpolation.
                hostIndex = i;
            }
#endif
        }

        if(hostIndex < 0) {
            hostIndex = Pa_GetDefaultHostApi();
        }
        auto hostInfo = Pa_GetHostApiInfo(hostIndex);
        FF_CONSOLE_LOG("Choosing host API: %s", hostInfo->name);

        for(int i = 0; i < Pa_GetDeviceCount(); i++) {
            auto deviceInfo = Pa_GetDeviceInfo(i);
            FF_CONSOLE_LOG("Device found: %s [%s]", deviceInfo->name, Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
        }

        PaStreamParameters outputParameters;
        outputParameters.channelCount = channels;
        outputParameters.hostApiSpecificStreamInfo = nullptr;
        outputParameters.device = hostInfo->defaultOutputDevice;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;

        auto deviceInfo = Pa_GetDeviceInfo(outputParameters.device);
        FF_CONSOLE_LOG("Choosing device: %s [%s]", deviceInfo->name, Pa_GetHostApiInfo(deviceInfo->hostApi)->name);

        err = Pa_IsFormatSupported(nullptr,
            &outputParameters,
            deviceInfo->defaultSampleRate); // Ignore requested sample rate, we'll use the one native to the device
        FF_ASSERT(err == paNoError, "Invalid format: %s", Pa_GetErrorText(err));

        PaStream* stream;
        err = Pa_OpenStream(&stream,
            nullptr,
            &outputParameters,
            deviceInfo->defaultSampleRate,
            paFramesPerBufferUnspecified,
            0,
            PAAudioBackend::portaudioCallback,
            this);
        FF_ASSERT(err == paNoError, "PortAudio failed to open stream: %s", Pa_GetErrorText(err));
        _stream = std::unique_ptr<PaStream, void(*)(PaStream*)>(stream, [](PaStream* ptr) {
            if(ptr != nullptr) {
                Pa_CloseStream(ptr);
            }
        });

        FF_CONSOLE_LOG("Stream opened.");
        auto streamInfo = Pa_GetStreamInfo(_stream.get());
        FF_CONSOLE_LOG("Sample rate: %s", streamInfo->sampleRate);
        FF_CONSOLE_LOG("Output latency (ms): %s", streamInfo->outputLatency * 1000);

        err = Pa_StartStream(_stream.get());
        FF_ASSERT(err == paNoError, "PortAudio failed to start stream: %s", Pa_GetErrorText(err));

        FF_CONSOLE_LOG("Stream started.");
        _running = true;
    }
    void PAAudioBackend::onUpdate(const float& dt) {
    }
    void PAAudioBackend::onKill() {
        if(_stream) {
            Pa_StopStream(_stream.get());
            _stream.reset();
        }
    }
    void PAAudioBackend::onRequestPause() {
        if(_stream) {
            Pa_StopStream(_stream.get());
            _running = false;
        }
    }
    void PAAudioBackend::onRequestResume() {
        if(_stream) {
            Pa_StartStream(_stream.get());
            _running = true;
        }
    }

    int PAAudioBackend::portaudioCallback(const void* inputBuffer,
        void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData) {
        PAAudioBackend* const self = (PAAudioBackend* const)userData;

        float* castedBuffer = (float*)outputBuffer;
        self->getAudioCore()->bufferFrames(castedBuffer,
            framesPerBuffer,
            self->getSampleRate());

        return paContinue;
    }
}

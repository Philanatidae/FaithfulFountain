/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-audio-oboe/OboeAudioBackend.hpp>

#include <ff/CVars.hpp>

#include <memory>
#include <oboe/Oboe.h>

namespace ff {
    OboeAudioBackend::OboeAudioBackend() {
    }
    OboeAudioBackend::~OboeAudioBackend() {
        closeStream();
    }
    
    int OboeAudioBackend::getSampleRate() const {
        return _stream->getSampleRate();
    }

    oboe::DataCallbackResult OboeAudioBackend::onAudioReady(oboe::AudioStream* audioStream,
        void* audioData,
        int32_t numFrames) {
        // We requested AudioFormat::Float so we assume we got it.
        // For production code always check what format
        // the stream has and cast to the appropriate type.
        // @todo Check that the types match?
        auto *outputData = static_cast<float*>(audioData);

        getAudioCore()->bufferFrames(outputData,
                numFrames,
                getSampleRate());

        return oboe::DataCallbackResult::Continue;
    }

    void OboeAudioBackend::onInitialize(const int& channels, const int& requestedSampleRate) {
        if(CVars::get<int>("audio_oboe_app_sample_rate") > -1) {
            oboe::DefaultStreamValues::SampleRate = CVars::get<int>("audio_oboe_app_sample_rate");
        }
        if(CVars::get<int>("audio_oboe_app_frames_per_burst") > -1) {
            oboe::DefaultStreamValues::FramesPerBurst = CVars::get<int>("audio_oboe_app_frames_per_burst");
        }

        builder.setDirection(oboe::Direction::Output);
        builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
        builder.setSharingMode(oboe::SharingMode::Shared); // Exclusive is lower latency but uses device by itself. Shared will go through system mixer.
        builder.setUsage(oboe::Usage::Game);
        builder.setFormat(oboe::AudioFormat::Float);
        builder.setDirection(oboe::Direction::Output);
        FF_ASSERT(channels == 1 || channels == 2, "Oboe audio backend does not support %s channels.", channels);
        builder.setChannelCount(channels == 1 ? oboe::ChannelCount::Mono : oboe::ChannelCount::Stereo); // @todo Stereo
        builder.setDataCallback(this);

        openStream();
    }
    void OboeAudioBackend::onUpdate(const float& dt) {
    }
    void OboeAudioBackend::onKill() {
        closeStream();
    }
    void OboeAudioBackend::onRequestPause() {
        closeStream();
    }
    void OboeAudioBackend::onRequestResume() {
        openStream();
    }

    void OboeAudioBackend::openStream() {
        if(_stream != nullptr) {
            FF_CONSOLE_WARN("Stream already open.");
            return;
        }

        FF_CONSOLE_LOG("Opening stream...");
        oboe::Result result = builder.openStream(_stream);
        FF_ASSERT(result == oboe::Result::OK, "Could not open Oboe stream.");

        oboe::AudioFormat format = _stream->getFormat();
        FF_CONSOLE_LOG("Oboe stream format is %s.", oboe::convertToText(format));

        FF_CONSOLE_LOG("Starting stream...");
        result = _stream->requestStart();
        FF_ASSERT(result == oboe::Result::OK, "Could not start Oboe stream.");

        FF_CONSOLE_LOG("Oboe sample rate: %s", getSampleRate());
        FF_CONSOLE_LOG("Oboe audio API: %s", _stream->getAudioApi() == oboe::AudioApi::OpenSLES
            ? "OpenSL ES" : "AAudio");
    }
    void OboeAudioBackend::closeStream() {
        if(_stream == nullptr) {
            return;
        }

        FF_CONSOLE_LOG("Stopping stream...");
        oboe::Result result = _stream->requestStop();
        FF_ASSERT(result == oboe::Result::OK, "Could not stop Oboe stream.");
        FF_CONSOLE_LOG("Closing stream...");
        result = _stream->close();
        FF_ASSERT(result == oboe::Result::OK, "Could not close Oboe stream.");
        FF_CONSOLE_LOG("Audio stream closed.");

        _stream.reset();
    }
}

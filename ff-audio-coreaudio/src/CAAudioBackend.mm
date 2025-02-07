/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-audio-coreaudio/CAAudioBackend.hmm>

#include <glm/glm.hpp>
#include <limits>
#include <numeric>

#import "Foundation/Foundation.h"

#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#import "AVFoundation/AVFoundation.h"
#endif

#include <ff/math/MathHelper.hpp>

#include <ff/Locator.hpp>

namespace ff {
    CAAudioBackend::CAAudioBackend()
        :_audioQueue(nullptr),
        _running(false) {
    }
    CAAudioBackend::~CAAudioBackend() {
        stopAudioQueue();
        disposeAudioQueue();
    }
    
    int CAAudioBackend::getSampleRate() const {
        return (int)_streamDescription.mSampleRate;
    }

    void CAAudioBackend::onInitialize(const int& channels, const int& requestedSampleRate) {
        #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        // Ambient sound is ideal for games, since it respects
        // the mute button and doesn't stop music apps from playing.
        // Maybe in the future we can have a system where background
        // music existing will pause system music but if the player
        // mutes music in the game it will just have sound effects
        // with the system music not being interrupted.
        FF_CONSOLE_LOG("Setting audio session category to Ambient.");
        AVAudioSession* audioSession = AVAudioSession.sharedInstance;
        {
            NSError* err;
            [audioSession setCategory:AVAudioSessionCategoryAmbient error:&err];
            FF_ASSERT(err == nil, "Could not set audio session category (%s).", [[err description] UTF8String]);
        }
        #endif

        _streamDescription = {};
        _streamDescription.mSampleRate = 48000.0f; // @todo We probably don't have to ignore requestedSampleRate
        _streamDescription.mFormatID = kAudioFormatLinearPCM;
        _streamDescription.mFormatFlags = kLinearPCMFormatFlagIsFloat; ///*kLinearPCMFormatFlagIsBigEndian | */kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
        _streamDescription.mBitsPerChannel = sizeof(float) * 8;
        _streamDescription.mChannelsPerFrame = channels;
        _streamDescription.mBytesPerFrame = _streamDescription.mBitsPerChannel / 8 * _streamDescription.mChannelsPerFrame;
        _streamDescription.mFramesPerPacket = 1;
        _streamDescription.mBytesPerPacket = _streamDescription.mBytesPerFrame * _streamDescription.mFramesPerPacket;

        createAudioQueue();
        startAudioQueue();
    }
    void CAAudioBackend::onUpdate(const float& dt) {
    }
    void CAAudioBackend::onKill() {
        if(_audioQueue) {
            AudioQueueStop(_audioQueue, YES);
        }
    }
    void CAAudioBackend::onRequestPause() {
        stopAudioQueue();
        disposeAudioQueue();
    }
    void CAAudioBackend::onRequestResume() {
        createAudioQueue();
        startAudioQueue();
    }

    void CAAudioBackend::createAudioQueue() {
        if(_audioQueue) {
            FF_CONSOLE_WARN("Audio queue already created.");
            return;
        }

        OSStatus err;

        // Most of the 0 and nullptr params here are for compressed sound formats etc.
        FF_CONSOLE_LOG("Creating output queue...");
        err = AudioQueueNewOutput(&_streamDescription,
            &CAAudioBackend::caCallback,
            this,
            nil,
            nil,
            0,
            &_audioQueue);
        FF_ASSERT(err == noErr, "Could not create output queue.");

        // Generate buffers holding at most 1/128th of a second of data.
        // This value is experimentally determined by running on macOS and
        // iOS.
        size_t bufferSize = MathHelper::nextPowerOfTwo<size_t>(_streamDescription.mBytesPerFrame * (_streamDescription.mSampleRate / 128));
        FF_CONSOLE_LOG("Allocating queue buffers...");
        for(int i = 0; i < _audioQueueBuffers.size(); i++) {
            err = AudioQueueAllocateBuffer(_audioQueue, (int)bufferSize, &_audioQueueBuffers[i]);
            FF_ASSERT(err == noErr, "Could not allocate buffer.");
        }

        FF_CONSOLE_LOG("Priming buffers...");
        for(int i = 0; i < _audioQueueBuffers.size(); i++) {
            caCallback(this,
                _audioQueue,
                _audioQueueBuffers[i]);
        }
    }
    void CAAudioBackend::startAudioQueue() {
        if(_running) {
            FF_CONSOLE_WARN("Audio queue already started.");
            return;
        }

        // Start playing
        FF_CONSOLE_LOG("Starting audio queue...");
        AudioQueueStart(_audioQueue, nil);
        _running = true;

        FF_CONSOLE_LOG("Audio queue started.");
    }
    void CAAudioBackend::stopAudioQueue() {
        if(!_running) {
            FF_CONSOLE_WARN("Audio queue already stopped.");
            return;
        }

        // Stop playing
        FF_CONSOLE_LOG("Stopping audio queue...");
        AudioQueueStop(_audioQueue, YES);
        _running = false;

        FF_CONSOLE_LOG("Audio queue stopped.");
    }
    void CAAudioBackend::disposeAudioQueue() {
        if(!_audioQueue) {
            FF_CONSOLE_WARN("Audio queue already disposed.");
            return;
        }

        // Dispose of the queue
        FF_CONSOLE_LOG("Disposing audio queue...");
        AudioQueueDispose(_audioQueue, YES);
        _audioQueue = nullptr;

        FF_CONSOLE_LOG("Audio queue disposed.");
    }

    void CAAudioBackend::caCallback(void* userData,
        AudioQueueRef queue,
        AudioQueueBufferRef buffer) {
        CAAudioBackend* const self = (CAAudioBackend* const)userData;
        float* const bufferData = (float*)buffer->mAudioData;
        
        int const samplesPerChannel = buffer->mAudioDataBytesCapacity / self->_streamDescription.mBytesPerFrame;
        buffer->mAudioDataByteSize = samplesPerChannel * self->_streamDescription.mBytesPerFrame;
        
        int const framesToGen = samplesPerChannel * self->_streamDescription.mChannelsPerFrame;
        
        self->getAudioCore()->bufferFrames(&bufferData[0], samplesPerChannel, self->_streamDescription.mSampleRate);
        
        AudioQueueEnqueueBuffer(queue, buffer, 0, 0);
    }
}

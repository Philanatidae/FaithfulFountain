/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "iOSGameLoop.hmm"

#include <stdlib.h>

#include <glm/glm.hpp>

namespace ff {
    iOSGameLoop::iOSGameLoop(ViewController* viewController,
        CADisplayLink* displayLink)
        :_viewController(viewController),
        _displayLink(displayLink),
        _loopTimeInit(false),
        _forceZeroDeltaTime(false) {
    }
    iOSGameLoop::~iOSGameLoop() {
    }
        
    void iOSGameLoop::init(ff::Game* const& gamePtr) {
    }
    float iOSGameLoop::computeFrameDeltaTime() {
        if(_forceZeroDeltaTime) {
            return 0;
        }
        if(!_loopTimeInit) {
            _prevLoopTime = std::chrono::steady_clock::now();
            _loopTimeInit = true;
            return 0;
        }
        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
        std::chrono::duration<float> dt = now - _prevLoopTime;
        _prevLoopTime = now;
        return dt.count();
    }
    void iOSGameLoop::update(const float& dt) {
    }
    void iOSGameLoop::onService() {
    }
    void iOSGameLoop::onShutdown() {
        FF_CONSOLE_LOG("Exiting...");
        exit(EXIT_SUCCESS);
    }
    float iOSGameLoop::getTargetRefreshRate() {
        // Need to round refresh rate, similar reason to macOS.
        return glm::round((float)(1.0 / (_displayLink.targetTimestamp - _displayLink.timestamp)));
    }
    glm::ivec2 iOSGameLoop::getBackBufferDimensions() {
        CGSize drawableSize = _viewController.view.frame.size;
        CGFloat scaleFactor = [UIScreen mainScreen].scale;
        return glm::ivec2((int)(drawableSize.width * scaleFactor), (int)(drawableSize.height * scaleFactor));
    }

    void iOSGameLoop::beginForceZeroDeltaTime() {
        _forceZeroDeltaTime = true;
    }
    void iOSGameLoop::endForceZeroDeltaTime() {
        _forceZeroDeltaTime = false;
    }
}

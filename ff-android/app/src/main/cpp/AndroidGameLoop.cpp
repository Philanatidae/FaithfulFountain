/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "AndroidGameLoop.hpp"

#include <stdlib.h>
#include <chrono>

#define FF_JNI_EXCEPTION_CHECK(jni) if((jni)->ExceptionCheck()) { FF_CONSOLE_FATAL("JNI had an exception."); }

namespace ff {
    AndroidGameLoop::AndroidGameLoop(struct android_app* app,
            JNIEnv* const& jni)
        :_app(app),
        _jni(jni) {
    }
    AndroidGameLoop::~AndroidGameLoop() {
    }

    struct android_app* AndroidGameLoop::getAndroidApp() const {
        return _app;
    }

    void AndroidGameLoop::init(ff::Game* const& gamePtr) {
    }
    void AndroidGameLoop::update(const float& dt) {
    }
    void AndroidGameLoop::onService() {
    }
    void AndroidGameLoop::onShutdown() {
        FF_CONSOLE_LOG("Exiting...");
        exit(EXIT_SUCCESS);
    }
    float AndroidGameLoop::getTargetRefreshRate() {
        jclass activityClass = _jni->GetObjectClass(_app->activity->clazz);
        jmethodID getTargetFrameRate = _jni->GetMethodID(activityClass, "getTargetFrameRate", "()F");

        float refreshRate = _jni->CallFloatMethod(_app->activity->clazz, getTargetFrameRate);

        _jni->DeleteLocalRef(activityClass);

        return refreshRate;
    }
    glm::ivec2 AndroidGameLoop::getBackBufferDimensions() {
        return _backBufferDimensions;
    }

    void AndroidGameLoop::setBackBufferDimensions(glm::ivec2 const& bb) {
        _backBufferDimensions = bb;
    }
}

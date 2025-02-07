/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ANDROID_GAME_LOOP_HPP
#define _FAITHFUL_FOUNTAIN_ANDROID_GAME_LOOP_HPP

#include <ff/GameServicer.hpp>
#include <chrono>

#include <android_native_app_glue.h>

namespace ff {
    class AndroidGameLoop : public ff::GameLoop {
    public:
        AndroidGameLoop(struct android_app* app,
            JNIEnv* const& jni);
        virtual ~AndroidGameLoop();

        struct android_app* getAndroidApp() const;

        void init(ff::Game* const& gamePtr) override;
        void update(const float& dt) override;
        void onService() override;
        void onShutdown() override;
        float getTargetRefreshRate() override;
        glm::ivec2 getBackBufferDimensions() override;

        void setBackBufferDimensions(glm::ivec2 const& bb);

    private:
        struct android_app* _app;
        JNIEnv* _jni;
        glm::ivec2 _backBufferDimensions;
    };
}

#endif

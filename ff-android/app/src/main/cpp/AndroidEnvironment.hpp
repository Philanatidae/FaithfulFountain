/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ANDROID_ANDROID_ENVIRONMENT_HPP
#define _FAITHFUL_FOUNTAIN_ANDROID_ANDROID_ENVIRONMENT_HPP

#include <ff/env/IEnvironment.hpp>
#include <ff/env/StdDataStorage.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/commands/env/EnvRequestOpenURLCommand.hpp>

#include <android_native_app_glue.h>
#include <jni.h>

#include "AndroidMotionEventTransmitter.hpp"
#include <memory>

namespace ff {
    class AndroidEnvironment : public IEnvironment,
       public EventListener<EnvRequestOpenURLCommand> {
    public:
        AndroidEnvironment(struct android_app* app,
                JNIEnv* const& jni,
                const std::string& configDirectory);
        ~AndroidEnvironment() = default;

        struct android_app* getAndroidApp() const;

        void init() override;
        bool isInitialized() const;

        std::string getName() const override;

        IDataStorage const& getDataStorage() const override;
        EnvironmentCapabilities_t getCapabilities() const override;

        std::string getConfigDirectory() const;

        void setReportGravity(bool const& enable) override;
        void setReportOrientation(bool const& enable) override;
        void setReportAcceleration(bool const& enable) override;

        bool isReportingGravity() const override;
        bool isReportingOrientation() const override;
        bool isReportingAcceleration() const override;

        std::string getClipboard() const override;

        bool isRecordingTextInput() const override;

        void processSensorEvents(int ident);
        void stopSensors();
        void startSensors();

    protected:
        bool processEvent(EnvRequestOpenURLCommand const& evt) override;

    private:
        struct android_app* _app;
        JNIEnv* _jni;
        std::string _configDirectory;
        StdDataStorage _dataStorage;
        bool _init;

        EnvironmentCapabilities_t _capabilities;

        std::unique_ptr<AndroidMotionEventTransmitter> _motionEventTransmitter;
    };
}

#endif

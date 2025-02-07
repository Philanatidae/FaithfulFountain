/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "AndroidEnvironment.hpp"

#include <ff/Console.hpp>
#include <ff/Locator.hpp>

#define FF_JNI_EXCEPTION_CHECK(jni) if((jni)->ExceptionCheck()) { FF_CONSOLE_FATAL("JNI had an exception."); }

namespace ff {
    AndroidEnvironment::AndroidEnvironment(struct android_app* app,
        JNIEnv* const& jni,
        const std::string& configDirectory)
        :_app(app),
        _jni(jni),
        _configDirectory(configDirectory),
        _dataStorage(getConfigDirectory()),
        _capabilities(0) {
    }

    struct android_app* AndroidEnvironment::getAndroidApp() const {
        return _app;
    }

    void AndroidEnvironment::init() {
        FF_CONSOLE_LOG("Config directory: `%s`", getConfigDirectory());

        Locator::getMessageBus().addListener<EnvRequestOpenURLCommand>(this);
        _capabilities |= EnvironmentCapabilities::OPEN_URL;

        _motionEventTransmitter = std::make_unique<AndroidMotionEventTransmitter>(_app->looper);
        if(_motionEventTransmitter->isGravityAvailable()) {
            FF_CONSOLE_LOG("Gravity sensor is available.");
            _capabilities |= EnvironmentCapabilities::MOTION_GRAVITY;
        } else {
            FF_CONSOLE_LOG("Gravity sensor is not available.");
        }
        if(_motionEventTransmitter->isOrientationAvailable()) {
            FF_CONSOLE_LOG("Orientation sensor is available.");
            _capabilities |= EnvironmentCapabilities::MOTION_ORIENTATION;
        } else {
            FF_CONSOLE_LOG("Orientation sensor is not available.");
        }
        if(_motionEventTransmitter->isAccelerationAvailable()) {
            FF_CONSOLE_LOG("Acceleration sensor is available.");
            _capabilities |= EnvironmentCapabilities::MOTION_ACCELERATION;
        } else {
            FF_CONSOLE_LOG("Acceleration sensor is not available.");
        }

        _init = true;
    }
    bool AndroidEnvironment::isInitialized() const {
        return _init;
    }

    std::string AndroidEnvironment::getName() const {
        return "Android";
    }

    IDataStorage const& AndroidEnvironment::getDataStorage() const {
        return _dataStorage;
    }

    std::string AndroidEnvironment::getConfigDirectory() const {
        return _configDirectory;
    }
    EnvironmentCapabilities_t AndroidEnvironment::getCapabilities() const {
        return _capabilities;
    }

    void AndroidEnvironment::setReportGravity(bool const& enable) {
        if(_motionEventTransmitter->isGravityAvailable()) {
            _motionEventTransmitter->setReportGravity(enable);
            return;
        }
        FF_CONSOLE_ERROR("Environment does not have gravity motion capability.");
    }
    void AndroidEnvironment::setReportOrientation(bool const& enable) {
        if(_motionEventTransmitter->isOrientationAvailable()) {
            _motionEventTransmitter->setReportOrientation(enable);
            return;
        }
        FF_CONSOLE_ERROR("Environment does not have orientation motion capability.");
    }
    void AndroidEnvironment::setReportAcceleration(bool const& enable) {
        if(_motionEventTransmitter->isAccelerationAvailable()) {
            _motionEventTransmitter->setReportAcceleration(enable);
            return;
        }
        FF_CONSOLE_ERROR("Environment does not have acceleration motion capability.");
    }

    bool AndroidEnvironment::isReportingGravity() const {
        return _motionEventTransmitter->isReportingGravity();
    }
    bool AndroidEnvironment::isReportingOrientation() const {
        return _motionEventTransmitter->isReportingOrientation();
    }
    bool AndroidEnvironment::isReportingAcceleration() const {
        return _motionEventTransmitter->isReportingAcceleration();
    }

    std::string AndroidEnvironment::getClipboard() const {
        // @todo Add clipboard support to Android
        return "";
    }

    bool AndroidEnvironment::isRecordingTextInput() const {
        return false; // @todo Add text recording support to Android
    }

    void AndroidEnvironment::processSensorEvents(int ident) {
        _motionEventTransmitter->processSensorEvents(ident);
    }
    void AndroidEnvironment::stopSensors() {
        _motionEventTransmitter->stopSensors();
    }
    void AndroidEnvironment::startSensors() {
        _motionEventTransmitter->startSensors();
    }

    bool AndroidEnvironment::processEvent(EnvRequestOpenURLCommand const& evt) {
        std::string fullUrl = tinyformat::format("https://%s", message.address);

        jstring javaURL = _jni->NewStringUTF(fullUrl.c_str());

        jclass activityClass = _jni->GetObjectClass(_app->activity->clazz);
        jmethodID openURL = _jni->GetMethodID(activityClass, "openURL", "(Ljava/lang/String;)V");

        _jni->CallVoidMethod(_app->activity->clazz, openURL, javaURL);

        _jni->DeleteLocalRef(activityClass);
        _jni->DeleteLocalRef(javaURL);
        
        return false;
    }
}

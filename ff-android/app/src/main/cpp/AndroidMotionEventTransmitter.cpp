/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "AndroidMotionEventTransmitter.hpp"

#include <ff/Console.hpp>
#include <ff/CVars.hpp>
#include <ff/Locator.hpp>

#include <ff/messages/MessageBus.hpp>
#include <ff/events/input/GravityChangedEvent.hpp>
#include <ff/events/input/AccelerationChangedEvent.hpp>

namespace ff {

AndroidMotionEventTransmitter::AndroidMotionEventTransmitter(ALooper* looper)
    :_reportingGravity(false),
    _reportingOrien(false),
    _reportingAccel(false) {
    _sensorManager = ASensorManager_getInstance();

    _gravitySensor = ASensorManager_getDefaultSensor(_sensorManager,
                                                     ASENSOR_TYPE_GRAVITY);
    _orienSensor = nullptr; // The sensor we need was deprecated and removed. When this is needed, come up with a way to re-create it.
    _accelSensor = ASensorManager_getDefaultSensor(_sensorManager,
                                                     ASENSOR_TYPE_ACCELEROMETER);

    _eventQueue = ASensorManager_createEventQueue(_sensorManager,
                                                  looper,
                                                  LOOPER_ID_USER,
                                                  nullptr,
                                                  nullptr);
}
AndroidMotionEventTransmitter::~AndroidMotionEventTransmitter() {
    if(isReportingGravity()) {
        setReportGravity(false);
    }
    if(isReportingOrientation()) {
        setReportOrientation(false);
    }
    if(isReportingAcceleration()) {
        setReportAcceleration(false);
    }
}

bool AndroidMotionEventTransmitter::isGravityAvailable() const {
    return _gravitySensor != nullptr;
}
bool AndroidMotionEventTransmitter::isOrientationAvailable() const {
    return _orienSensor != nullptr;
}
bool AndroidMotionEventTransmitter::isAccelerationAvailable() const {
    return _accelSensor != nullptr;
}

void AndroidMotionEventTransmitter::processSensorEvents(int ident) {
    if(ident != LOOPER_ID_USER) {
        return;
    }

    ASensorEvent event;
    while(ASensorEventQueue_getEvents(_eventQueue,
                                      &event,
                                      1) > 0) {
        if(event.type == ASENSOR_TYPE_GRAVITY) {
            if(!isGravityAvailable()) {
                ASensorEventQueue_disableSensor(_eventQueue, _gravitySensor);
                return;
            }
            glm::vec3 vec(event.data[0], event.data[1], event.data[2]);
            vec = glm::normalize(vec);
            Locator::getMessageBus().dispatch<GravityChangedEvent>(vec);
        }
        if(event.type == ASENSOR_TYPE_ACCELEROMETER) {
            if(!isAccelerationAvailable()) {
                ASensorEventQueue_disableSensor(_eventQueue, _accelSensor);
                return;
            }
            glm::vec3 vec(event.data[0], event.data[1], event.data[2]);
            vec = glm::normalize(vec);
            Locator::getMessageBus().dispatch<AccelerationChangedEvent>(vec);
        }
    }
}


void AndroidMotionEventTransmitter::setReportGravity(bool const& enabled) {
    if(!isGravityAvailable()) {
        return;
    }

    if(enabled) {
        if(isReportingGravity()) {
            return;
        }

        FF_CONSOLE_LOG("Enabling acceleration sensor.");
        ASensorEventQueue_enableSensor(_eventQueue,
                                       _gravitySensor);
        ASensorEventQueue_setEventRate(_eventQueue,
                                       _gravitySensor,
                                       (int32_t)(1000.0f / CVars::get<float>("android_motion_update_frequency") * 1000.0f));
        _reportingGravity = true;
    } else {
        if(!isReportingGravity()) {
            FF_CONSOLE_LOG("Disabling acceleration sensor.");
            ASensorEventQueue_disableSensor(_eventQueue,
                                            _gravitySensor);
            _reportingGravity = false;
        }
    }
}
void AndroidMotionEventTransmitter::setReportOrientation(bool const& enabled) {
    FF_CONSOLE_ERROR("Orientation sensor is not implemented.");
}
void AndroidMotionEventTransmitter::setReportAcceleration(bool const& enabled) {
    if(!isAccelerationAvailable()) {
        return;
    }

    if(enabled) {
        if(isReportingAcceleration()) {
            return;
        }

        FF_CONSOLE_LOG("Enabling acceleration sensor.");
        ASensorEventQueue_enableSensor(_eventQueue,
                                       _accelSensor);
        ASensorEventQueue_setEventRate(_eventQueue,
                                       _accelSensor,
                                       (int32_t)(1000.0f / CVars::get<float>("android_motion_update_frequency") * 1000.0f));
        _reportingAccel = true;
    } else {
        if(!isReportingAcceleration()) {
            FF_CONSOLE_LOG("Disabling acceleration sensor.");
            ASensorEventQueue_disableSensor(_eventQueue,
                                            _accelSensor);
            _reportingAccel = false;
        }
    }
}

bool AndroidMotionEventTransmitter::isReportingGravity() const {
    return _reportingGravity;
}
bool AndroidMotionEventTransmitter::isReportingOrientation() const {
    return _reportingOrien;
}
bool AndroidMotionEventTransmitter::isReportingAcceleration() const {
    return _reportingAccel;
}

void AndroidMotionEventTransmitter::stopSensors() const {
    FF_CONSOLE_LOG("Stopping sensors...");
    if(isReportingGravity()) {
        ASensorEventQueue_disableSensor(_eventQueue,
                                        _gravitySensor);
    }
    if(isReportingOrientation()) {
        ASensorEventQueue_disableSensor(_eventQueue,
                                        _orienSensor);
    }
    if(isReportingAcceleration()) {
        ASensorEventQueue_disableSensor(_eventQueue,
                                        _accelSensor);
    }
}
void AndroidMotionEventTransmitter::startSensors() const {
    FF_CONSOLE_LOG("Starting sensors...");
    if(isReportingGravity()) {
        ASensorEventQueue_enableSensor(_eventQueue,
                                        _gravitySensor);
    }
    if(isReportingOrientation()) {
        ASensorEventQueue_enableSensor(_eventQueue,
                                        _orienSensor);
    }
    if(isReportingAcceleration()) {
        ASensorEventQueue_enableSensor(_eventQueue,
                                        _accelSensor);
    }
}

}

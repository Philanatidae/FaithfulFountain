/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ANDROID_MOTION_EVENT_TRANSMITTER_HPP
#define _FAITHFUL_FOUNTAIN_ANDROID_MOTION_EVENT_TRANSMITTER_HPP

#include <android_native_app_glue.h>
#include <android/sensor.h>

namespace ff {

class AndroidMotionEventTransmitter final {
public:
    AndroidMotionEventTransmitter(ALooper* looper);
    ~AndroidMotionEventTransmitter();

    bool isGravityAvailable() const;
    bool isOrientationAvailable() const;
    bool isAccelerationAvailable() const;

    void setReportGravity(bool const& enabled);
    void setReportOrientation(bool const& enabled);
    void setReportAcceleration(bool const& enabled);

    bool isReportingGravity() const;
    bool isReportingOrientation() const;
    bool isReportingAcceleration() const;

    void stopSensors() const;
    void startSensors() const;

    void processSensorEvents(int ident);

private:
    ASensorManager* _sensorManager;
    ASensorEventQueue* _eventQueue;

    ASensorRef _gravitySensor;
    ASensorRef _orienSensor;
    ASensorRef _accelSensor;

    bool _reportingGravity;
    bool _reportingOrien;
    bool _reportingAccel;
};

}

#endif

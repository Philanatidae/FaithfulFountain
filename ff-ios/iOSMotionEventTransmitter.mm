/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "iOSMotionEventTransmitter.hmm"

#include <ff/messages/MessageBus.hpp>

#include <ff/events/input/GravityChangedEvent.hpp>
#include <ff/events/input/OrientationChangedEvent.hpp>
#include <ff/events/input/AccelerationChangedEvent.hpp>

#include <ff/CVars.hpp>

namespace ff {

iOSMotionEventTransmitter::iOSMotionEventTransmitter()
    :_reportingGravity(false),
    _reportingOrientation(false),
    _reportingAcceleration(false) {
    _motionManager = [CMMotionManager new];
}
iOSMotionEventTransmitter::~iOSMotionEventTransmitter() {
    stopReporting();
}

bool iOSMotionEventTransmitter::isMotionAvailable() const {
    return _motionManager != nil && [_motionManager isDeviceMotionAvailable];
}

void iOSMotionEventTransmitter::setReportGravity(bool const& enabled) {
    if(isReportingGravity() && enabled) {
        return;
    }
    if(!isReportingGravity() && !enabled) {
        return;
    }
    
    _reportingGravity = enabled;
    FF_CONSOLE_LOG("Gravity reporting %s.", enabled ? "enabled" : "disabled");
    
    if(enabled && !isReporting()) {
        startReporting();
    }
    if(!isReportingGravity()
       && !isReportingOrientation()
       && !isReportingAcceleration()) {
        stopReporting();
    }
}
void iOSMotionEventTransmitter::setReportOrientation(bool const& enabled) {
    if(isReportingOrientation() && enabled) {
        return;
    }
    if(!isReportingOrientation() && !enabled) {
        return;
    }
    
    _reportingOrientation = enabled;
    FF_CONSOLE_LOG("Orientation reporting %s.", enabled ? "enabled" : "disabled");
    
    if(enabled && !isReporting()) {
        startReporting();
    }
    if(!isReportingGravity()
       && !isReportingOrientation()
       && !isReportingAcceleration()) {
        stopReporting();
    }
}
void iOSMotionEventTransmitter::setReportAcceleration(bool const& enabled) {
    if(isReportingAcceleration() && enabled) {
        return;
    }
    if(!isReportingAcceleration() && !enabled) {
        return;
    }
    
    _reportingAcceleration = enabled;
    FF_CONSOLE_LOG("Acceleration reporting %s.", enabled ? "enabled" : "disabled");
    
    if(enabled && !isReporting()) {
        startReporting();
    }
    if(!isReportingGravity()
       && !isReportingOrientation()
       && !isReportingAcceleration()) {
        stopReporting();
    }
}

bool iOSMotionEventTransmitter::isReportingGravity() const {
    return _reportingGravity;
}
bool iOSMotionEventTransmitter::isReportingOrientation() const {
    return _reportingOrientation;
}
bool iOSMotionEventTransmitter::isReportingAcceleration() const {
    return _reportingAcceleration;
}

void iOSMotionEventTransmitter::startReporting() {
    if(_motionManager.isDeviceMotionActive) {
        return;
    }
    
    _motionManager.deviceMotionUpdateInterval = 1/CVars::get<float>("ios_motion_update_frequency");
    
    // @todo Prompt needs to be allowed to be shown. It would
    // be ideal if there is a way to manually trigger this,
    // or to be informed when the prompt is displayed
    // so that we can transmit an event.
    _motionManager.showsDeviceMovementDisplay = NO;

    FF_CONSOLE_LOG("Device motion update capture started.");
    [_motionManager startDeviceMotionUpdatesToQueue:[NSOperationQueue mainQueue] withHandler:^(CMDeviceMotion* motion, NSError* error) {
        handleMotion(motion);
    }];
}
void iOSMotionEventTransmitter::stopReporting() {
    if(!_motionManager.isDeviceMotionActive) {
        return;
    }
    
    FF_CONSOLE_LOG("Device motion update capture stopped.");
    [_motionManager stopDeviceMotionUpdates];
}
bool iOSMotionEventTransmitter::isReporting() const {
    return _motionManager.isDeviceMotionActive;
}

void iOSMotionEventTransmitter::handleMotion(CMDeviceMotion* motion) {
    if(isReportingGravity()) {
        Locator::getMessageBus().dispatch<GravityChangedEvent>(glm::vec3(motion.gravity.x,
            motion.gravity.y,
            motion.gravity.z));
    }
    if(isReportingOrientation()) {
        Locator::getMessageBus().dispatch<OrientationChangedEvent>(glm::quat(motion.attitude.quaternion.w,
            motion.attitude.quaternion.x,
            motion.attitude.quaternion.y,
            motion.attitude.quaternion.z));
    }
    if(isReportingAcceleration()) {
        Locator::getMessageBus().dispatch<AccelerationChangedEvent>(glm::vec3(motion.userAcceleration.x,
           motion.userAcceleration.y,
           motion.userAcceleration.z));
    }
}

}

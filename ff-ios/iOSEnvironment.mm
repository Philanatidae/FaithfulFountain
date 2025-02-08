/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "iOSEnvironment.hmm"

#include <ff/CVars.hpp>

#import <Foundation/Foundation.h>

#include <ff-support-apple/Helpers.hpp>

namespace ff {
    iOSEnvironment::iOSEnvironment()
        :_dataStorage(getConfigDirectory()),
        _capabilities(EnvironmentCapabilities::OPEN_URL) {
    }
    iOSEnvironment::~iOSEnvironment() {
    }

    void iOSEnvironment::init() {
        FF_CONSOLE_LOG("Config directory: `%s`", getConfigDirectory());
        
        Locator::getMessageBus().addListener<EnvRequestOpenURLCommand>(this);

        
        _motionEventTransmitter = std::make_unique<iOSMotionEventTransmitter>();
        if(!_motionEventTransmitter->isMotionAvailable()) {
            FF_CONSOLE_WARN("CoreMotion is not available.");
        } else {
            _capabilities |= EnvironmentCapabilities::MOTION_GRAVITY;
            _capabilities |= EnvironmentCapabilities::MOTION_ACCELERATION;
            _capabilities |= EnvironmentCapabilities::MOTION_ORIENTATION;
        }
    }

    std::string iOSEnvironment::getName() const {
        return "iOS";
    }
    EnvironmentCapabilities_t iOSEnvironment::getCapabilities() const {
        if(_motionEventTransmitter->isMotionAvailable()) {
            return EnvironmentCapabilities::MOTION_GRAVITY
                | EnvironmentCapabilities::MOTION_ACCELERATION
                | EnvironmentCapabilities::MOTION_ORIENTATION;
        }
        return 0;
    }

    IDataStorage const& iOSEnvironment::getDataStorage() const {
        return _dataStorage;
    }

    void iOSEnvironment::setReportGravity(bool const& enable) {
        if(_motionEventTransmitter->isMotionAvailable()) {
            _motionEventTransmitter->setReportGravity(enable);
            return;
        }
        FF_CONSOLE_ERROR("Environment does not have gravity motion capability.");
    }
    void iOSEnvironment::setReportOrientation(bool const& enable) {
        if(_motionEventTransmitter->isMotionAvailable()) {
            _motionEventTransmitter->setReportOrientation(enable);
            return;
        }
        FF_CONSOLE_ERROR("Environment does not have orientation motion capability.");
    }
    void iOSEnvironment::setReportAcceleration(bool const& enable) {
        if(_motionEventTransmitter->isMotionAvailable()) {
            _motionEventTransmitter->setReportAcceleration(enable);
            return;
        }
        FF_CONSOLE_ERROR("Environment does not have acceleration motion capability.");
    }

    bool iOSEnvironment::isReportingGravity() const {
        if(_motionEventTransmitter->isMotionAvailable()) {
            return _motionEventTransmitter->isReportingGravity();
        }
        return false;
    }
    bool iOSEnvironment::isReportingOrientation() const {
        if(_motionEventTransmitter->isMotionAvailable()) {
            return _motionEventTransmitter->isReportingOrientation();
        }
        return false;
    }
    bool iOSEnvironment::isReportingAcceleration() const {
        if(_motionEventTransmitter->isMotionAvailable()) {
            return _motionEventTransmitter->isReportingAcceleration();
        }
        return false;
    }

    std::string iOSEnvironment::getConfigDirectory() const {
        // NSHomeDirectory will respect sandbox.
        return std::filesystem::path([NSHomeDirectory() UTF8String])/"Library"/"Application Support"/CVars::get<std::string>("package_prefix")/CVars::get<std::string>("game_name");
    }

    void iOSEnvironment::processEvent(EnvRequestOpenURLCommand const& evt) {
        Helpers::openURL(message.address);
    }
}

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/directors/DebugCameraDirector.hpp>

#include <ff/Locator.hpp>
#include <ff/CVars.hpp>

#include <ff/math/VectorHelper.hpp>

#include <ff/commands/input/RelativeMouseMovementCmd.hpp>
#include <ff/commands/input/AbsoluteMouseMovementCmd.hpp>

namespace ff {

DebugCameraDirector::DebugCameraDirector(std::string const& camera)
    :_camera(camera),
    _isMoving(false),
    _relVel(0.0f) {
}

bool DebugCameraDirector::processEvent(MouseButtonDownEvent const& evt) {
    if(!_isMoving && evt.button == MouseButton::SECONDARY) {
        FF_CONSOLE_LOG("Begin debug camera movement");
        _isMoving = true;
        _relVel.x = _relVel.y = _relVel.z = 0.0f;
        _relMouseSum.x = _relMouseSum.y = 0.0f;
        _hadPress.clear();
        Locator::getMessageBus().dispatch<RelativeMouseMovementCmd>();
        return true;
    }
    return false;
}
bool DebugCameraDirector::processEvent(MouseButtonUpEvent const& evt) {
    if(_isMoving && evt.button == MouseButton::SECONDARY) {
        FF_CONSOLE_LOG("End debug camera movement");
        _isMoving = false;
        _relVel.x = _relVel.y = _relVel.z = 0.0f;
        _relMouseSum.x = _relMouseSum.y = 0.0f;
        Locator::getMessageBus().dispatch<AbsoluteMouseMovementCmd>();
        return true;
    }
    return false;
}
bool DebugCameraDirector::processEvent(MouseMoveEvent const& evt) {
    if(_isMoving) {
        if(evt.type == MouseMoveType::RELATIVE) {
            _relMouseSum += evt.position;
        }
        else {
            Locator::getMessageBus().dispatch<RelativeMouseMovementCmd>();
        }
        return true;
    }
    return false;
}
bool DebugCameraDirector::processEvent(KeyboardKeyDownEvent const& evt) {
    if(_isMoving) {
        _hadPress.insert(evt.key);
        switch(evt.key) {
        case KeyboardKey::W:
            _relVel += VectorHelper::cameraLocalForward();
            return true;
        case KeyboardKey::S:
            _relVel -= VectorHelper::cameraLocalForward();
            return true;
        case KeyboardKey::A:
            _relVel += VectorHelper::cameraLocalRight();
            return true;
        case KeyboardKey::D:
            _relVel -= VectorHelper::cameraLocalRight();
            return true;
        case KeyboardKey::Q:
        case KeyboardKey::SPACE:
            _relVel += VectorHelper::cameraLocalUp();
            return true;
        case KeyboardKey::Z:
        case KeyboardKey::LEFT_CONTROL:
            _relVel -= VectorHelper::cameraLocalUp();
            return true;
        default:
            break;
        }

        return true;
    }
    return false;
}
bool DebugCameraDirector::processEvent(KeyboardKeyUpEvent const& evt) {
    if(_isMoving && _hadPress.find(evt.key) != _hadPress.end()) {
        switch(evt.key) {
        case KeyboardKey::W:
            _relVel -= VectorHelper::cameraLocalForward();
            return true;
        case KeyboardKey::S:
            _relVel += VectorHelper::cameraLocalForward();
            return true;
        case KeyboardKey::A:
            _relVel -= VectorHelper::cameraLocalRight();
            return true;
        case KeyboardKey::D:
            _relVel += VectorHelper::cameraLocalRight();
            return true;
        case KeyboardKey::Q:
        case KeyboardKey::SPACE:
            _relVel -= VectorHelper::cameraLocalUp();
            return true;
        case KeyboardKey::Z:
        case KeyboardKey::LEFT_CONTROL:
            _relVel += VectorHelper::cameraLocalUp();
            return true;
        default:
            break;
        }

        return true;
    }
    return false;
}

void DebugCameraDirector::onInitialize() {
    Locator::getMessageBus().addListener<MouseButtonDownEvent>(this);
    Locator::getMessageBus().addListener<MouseButtonUpEvent>(this);
    Locator::getMessageBus().addListener<MouseMoveEvent>(this);
    Locator::getMessageBus().addListener<KeyboardKeyDownEvent>(this);
    Locator::getMessageBus().addListener<KeyboardKeyUpEvent>(this);
}
void DebugCameraDirector::onUpdate(const float& dt) {
    PerspectiveCamera* dcam = Locator::getCameraManager().getPCamera(_camera);
    if(dcam == nullptr) {
        return;
    }
    if(_isMoving) {
        glm::vec3 const dir = dcam->getRotation() * _relVel;
        dcam->setPosition(dcam->getPosition() + dir * CVars::get<float>("debug_cam_speed") * dt);
        
        dcam->setRotationEuler(dcam->getRotationEuler() + glm::vec3(-_relMouseSum.y, 0, -_relMouseSum.x) * CVars::get<float>("debug_cam_sensitivity") * dt);
        _relMouseSum.x = _relMouseSum.y = 0.0f;

        dcam->update();
    }
}

}


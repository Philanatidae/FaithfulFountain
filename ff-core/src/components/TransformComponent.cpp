/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/components/TransformComponent.hpp>

#include <ff/Console.hpp>

#include <glm/ext/matrix_common.hpp>
#include <glm/gtx/string_cast.hpp>

namespace ff {
    TransformComponent::TransformComponent(const glm::vec3& position,
        const glm::quat& rotation,
        const float& scale,
        const glm::vec3& stretch)
        :_position(position),_lastPosition(position),
        _rotation(rotation),_lastRotation(rotation),
        _scale(scale),_lastScale(scale),
        _stretch(stretch),_lastStretch(stretch) {
    }
    TransformComponent::TransformComponent(const TransformComponent& transformComp)
        :_position(transformComp._position),
        _lastPosition(transformComp._lastPosition),
        _rotation(transformComp._rotation),
        _lastRotation(transformComp._lastRotation),
        _scale(transformComp._scale),
        _lastScale(transformComp._lastScale),
        _stretch(transformComp._stretch),
        _lastStretch(transformComp._lastStretch) {
    }

    void TransformComponent::setPosition(const glm::vec3& position, const bool& reset) {
        _position = position;
        if(reset) {
            _lastPosition = position;
        }
    }
    void TransformComponent::setPosition(const float& x, const float& y, const bool& reset) {
        _position.x = x;
        _position.y = y;
        if(reset) {
            _lastPosition = _position;
        }
    }
    void TransformComponent::setPosition(const float& x, const float& y, const float& z, const bool& reset) {
        _position.x = x;
        _position.y = y;
        _position.z = z;
        if(reset) {
            _lastPosition = _position;
        }
    }

    void TransformComponent::setPositionX(const float& x, const bool& reset) {
        _position.x = x;
        if(reset) {
            _lastPosition = _position;
        }
    }
    void TransformComponent::setPositionY(const float& y, const bool& reset) {
        _position.y = y;
        if(reset) {
            _lastPosition = _position;
        }
    }
    void TransformComponent::setPositionZ(const float& z, const bool& reset) {
        _position.z = z;
        if(reset) {
            _lastPosition = _position;
        }
    }

    glm::vec3 TransformComponent::getPosition() const {
        return _position;
    }
    glm::vec2 TransformComponent::get2DPosition() const {
        return glm::vec2(_position.x, _position.y);
    }
    glm::vec3 TransformComponent::getLastPosition() const {
        return _lastPosition;
    }
    glm::vec2 TransformComponent::getLast2DPosition() const {
        return _lastPosition;
    }

    void TransformComponent::move(const glm::vec2& dp) {
        move(dp.x, dp.y);
    }
    void TransformComponent::move(const glm::vec3& dp) {
        move(dp.x, dp.y, dp.z);
    }
    void TransformComponent::move(const float& x, const float& y, const float& z) {
        _position.x += x;
        _position.y += y;
        _position.z += z;
    }

    void TransformComponent::set2DRotation(const float& rotation, const bool& reset) {
        setRotation(0, 0, rotation, reset);
    }
    void TransformComponent::rotate2D(const float& angle) {
        rotate(angle, 0, 0, 1);
    }
    float TransformComponent::get2DRotation() const {
        return glm::roll(_rotation);
    }


    void TransformComponent::setRotation(const glm::vec3& eulerAngles, const bool& reset) {
        _rotation = glm::quat(eulerAngles);
        if(reset) {
            _lastRotation = _rotation;
        }
    }
    void TransformComponent::setRotation(const float& roll, const float& pitch, const float& yaw, const bool& reset) {
        setRotation(glm::vec3(roll, pitch, yaw), reset);
    }
    void TransformComponent::setRotation(const glm::quat& quat, const bool& reset) {
        _rotation = quat;
        if(reset) {
            _lastRotation = _rotation;
        }
    }
    void TransformComponent::rotate(const float& angle, const glm::vec3& axis) {
        _rotation = glm::rotate(_rotation, angle, axis);
    }
    void TransformComponent::rotate(const float& angle, const float& x, const float& y, const float& z) {
        rotate(angle, glm::vec3(x, y, z));
    }
    glm::quat TransformComponent::getRotation() const {
        return _rotation;
    }
    glm::vec3 TransformComponent::getEulerRotation() const {
        return glm::eulerAngles(_rotation);
    }

    void TransformComponent::setScale(const float& scale, const bool& reset) {
        _scale = scale;
        if(reset) {
            _lastScale = scale;
        }
    }
    float TransformComponent::getScale() const {
        return _scale;
    }

    void TransformComponent::setStretch(const glm::vec2& stretch, const bool& reset) {
        setStretch(stretch.x, stretch.y, reset);
    }
    void TransformComponent::setStretch(const glm::vec3& stretch, const bool& reset) {
        setStretch(stretch.x, stretch.y, stretch.z, reset);
    }
    void TransformComponent::setStretch(const float& x, const float& y, const bool& reset) {
        _stretch.x = x;
        _stretch.y = y;
        if(reset) {
            _lastStretch = _stretch;
        }
    }
    void TransformComponent::setStretch(const float& x, const float& y, const float& z, const bool& reset) {
        _stretch.x = x;
        _stretch.y = y;
        _stretch.z = z;
        if(reset) {
            _lastStretch = _stretch;
        }
    }
    void TransformComponent::setStretchX(const float& x, const bool& reset) {
        setStretch(x, _stretch.y, reset);
    }
    void TransformComponent::setStretchY(const float& y, const bool& reset) {
        setStretch(_stretch.x, y, reset);
    }
    void TransformComponent::setStretchZ(const float& z, const bool& reset) {
        setStretch(_stretch.x, _stretch.y, _stretch.z, reset);
    }
    glm::vec3 TransformComponent::getStretch() const {
        return _stretch;
    }
    glm::vec2 TransformComponent::get2DStretch() const {
        return glm::vec2(_stretch.x, _stretch.y);
    }

    glm::mat4 TransformComponent::getMatrix() const {
        glm::mat4 mat = glm::identity<glm::mat4>();

        // Order: Stretch --> Scale --> Rotate --> Translate
        mat = glm::scale(mat, glm::vec3(_stretch.x * _scale, _stretch.y * _scale, _stretch.z * _scale));
        mat = glm::mat4x4(_rotation) * mat;
        mat = glm::translate(mat, _position);

        return mat;
    }
    glm::mat4 TransformComponent::getLastMatrix() const {
        glm::mat4 mat = glm::identity<glm::mat4>();

        // Order: Stretch --> Scale --> Rotate --> Translate
        mat = glm::scale(mat, glm::vec3(_lastStretch.x * _lastScale, _lastStretch.y * _lastScale, _lastStretch.z * _lastScale));
        mat = glm::mat4x4(_lastRotation) * mat;
        mat = glm::translate(mat, _lastPosition);

        return mat;
    }

    void TransformComponent::getSmoothedProperties(glm::vec3& pos,
        glm::quat& rotation,
        float& scale,
        glm::vec3& stretch,
        const float& alpha) {
        pos = glm::mix(_lastPosition, _position, 1 - alpha);
        rotation = glm::mix(_lastRotation, _rotation, 1 - alpha);
        scale = glm::mix(_lastScale, _scale, 1 - alpha);
        stretch = glm::mix(_lastStretch, _stretch, 1 - alpha);
    }
    glm::mat4 TransformComponent::getSmoothedMatrix(const float& alpha) {
        glm::vec3 pos;
        glm::quat rot;
        float scal;
        glm::vec3 stret;
        getSmoothedProperties(pos, rot, scal, stret, alpha);

        glm::mat4 mat = glm::identity<glm::mat4>();

        // Order: Stretch --> Scale --> Rotate --> Translate
        mat = glm::scale(glm::identity<glm::mat4>(), glm::vec3(stret.x * scal, stret.y * scal, stret.z * scal)) * mat;
        mat = glm::mat4_cast(rot) * mat;
        mat = glm::translate(glm::identity<glm::mat4>(), pos) * mat;

        return mat;
    }

    void TransformComponent::mirror(const TransformComponent &other, const bool& position, const bool& rotation, const bool& scale, const bool& stretch) {
        if(position) {
            _position = other._position;
            _lastPosition = other._lastPosition;
        }
        if(rotation) {
            _rotation = other._rotation;
            _lastRotation = other._lastRotation;
        }
        if(scale) {
            _scale = other._scale;
            _lastScale = other._lastScale;
        }
        if(stretch) {
            _stretch = other._stretch;
            _lastStretch = other._lastStretch;
        }
    }

    void TransformComponent::resetAll() {
        _lastPosition = _position;
        _lastRotation = _rotation;
        _lastScale = _scale;
        _lastStretch = _stretch;
    }
}

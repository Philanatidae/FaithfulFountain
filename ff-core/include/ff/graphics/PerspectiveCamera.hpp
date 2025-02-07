/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_PERSPECTIVE_CAMERA_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_PERSPECTIVE_CAMERA_HPP

#include <ff/graphics/Camera.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <ff/events/ResizeEvent.hpp>
#include <ff/messages/EventListener.hpp>

#include <optional>

namespace ff {

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(float const& fovy,
        float const& near,
        bool const& generateUniformBuffer = true);
    PerspectiveCamera(float const& fovy,
        float const& aspectRatio,
        float const& near,
        bool const& generateUniformBuffer = true);
    virtual ~PerspectiveCamera();

    glm::mat4x4 getTransformMatrix() const override;
    glm::mat4x4 getProjectionMatrix() const override;

    bool isGreaterZDepthIncrease() const override;
    bool isInfinite() const;

    glm::vec3 getPosition() const;
    void setPosition(glm::vec3 const& position);
    glm::quat getRotation() const;
    void setRotation(glm::quat const& rotation);
    glm::vec3 getRotationEuler() const;
    void setRotationEuler(glm::vec3 const& rotation);

    void lookAt(glm::vec3 const& eye, glm::vec3 const& lookAt);
    glm::vec3 getLookAt() const;

    float getFOVY() const;
    float getNear() const;
    
    void adjust(float const& fovy,
        float const& near,
        std::optional<float> const& aspectRatio = std::nullopt);
    void update();

private:
    glm::mat4x4 _projectionMatrix;
    glm::mat4x4 _transformMatrix;

    float _fovy;
    float _near;
    float _aspectRatio;

    glm::vec3 _position;
    glm::quat _rotation;

    void updateTransformMatrix();
};

}

#endif

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/PerspectiveCamera.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <ff/math/VectorHelper.hpp>
#include <ff/math/MatrixHelper.hpp>

#include <ff/Locator.hpp>

namespace ff {

PerspectiveCamera::PerspectiveCamera(float const& fovy,
    float const& near,
    bool const& generateUniformBuffer)
    :PerspectiveCamera(fovy,
        Locator::getGraphicsDevice().getBackBufferDimensions().x / (float)Locator::getGraphicsDevice().getBackBufferDimensions().y,
        near,
        generateUniformBuffer) {
}
PerspectiveCamera::PerspectiveCamera(float const& fovy,
    float const& aspectRatio,
    float const& near,
    bool const& generateUniformBuffer)
    :Camera(generateUniformBuffer),
    _position(0, 0, 0),
    _rotation(glm::identity<glm::quat>()),
    _fovy(-1),
    _near(-1),
    _aspectRatio(-1) {
    adjust(fovy, near, aspectRatio);
}
PerspectiveCamera::~PerspectiveCamera() {
}

glm::mat4x4 PerspectiveCamera::getTransformMatrix() const {
    return _transformMatrix;
}
glm::mat4x4 PerspectiveCamera::getProjectionMatrix() const {
    return _projectionMatrix;
}

bool PerspectiveCamera::isGreaterZDepthIncrease() const {
    return false;
}
bool PerspectiveCamera::isInfinite() const {
    return true;
}

glm::vec3 PerspectiveCamera::getPosition() const {
    return _position;
}
void PerspectiveCamera::setPosition(glm::vec3 const& position) {
    _position = position;
}
glm::quat PerspectiveCamera::getRotation() const {
    return _rotation;
}
void PerspectiveCamera::setRotation(glm::quat const& rotation) {
    _rotation = rotation;
}
glm::vec3 PerspectiveCamera::getRotationEuler() const {
    return glm::eulerAngles(_rotation);
}
void PerspectiveCamera::setRotationEuler(glm::vec3 const& rotation) {
    _rotation = glm::quat(rotation);
}

void PerspectiveCamera::lookAt(glm::vec3 const& eye, glm::vec3 const& lookAt) {
    _position = eye;

    glm::vec3 const direction = glm::normalize(lookAt - eye);
    _rotation = glm::quatLookAt(direction,
        VectorHelper::up());
}
glm::vec3 PerspectiveCamera::getLookAt() const {
    return _rotation * VectorHelper::cameraLocalForward();
}

float PerspectiveCamera::getFOVY() const { 
    return _fovy;
}
float PerspectiveCamera::getNear() const {
    return _near;
}

void PerspectiveCamera::adjust(float const& fovy,
    float const& near,
    std::optional<float> const& aspectRatio) {
    _fovy = fovy;
    _near = near;
    if(aspectRatio) {
        _aspectRatio = aspectRatio.value();
    }

    FF_ASSERT(_fovy > 0, "FOVY must be greater than 0 (FOVY = %s).", _fovy);
    FF_ASSERT(_near > 0, "Near must be greater than 0 (Near = %s).", _near);
    FF_ASSERT(_aspectRatio > 0, "Aspect ratio must be greater than 0 (Aspect ratio = %s).", _fovy);

    _projectionMatrix = MatrixHelper::createProjectionInfiniteReverse(Locator::getGraphicsDevice().getClipSpaceHandedness(),
        _fovy, _aspectRatio, _near);
}
void PerspectiveCamera::update() {
    updateTransformMatrix();
    updateCombinedMatrix(_projectionMatrix * _transformMatrix);
}

void PerspectiveCamera::updateTransformMatrix() {
    _transformMatrix = glm::identity<glm::mat4x4>();
    _transformMatrix = glm::mat4_cast(glm::conjugate(_rotation)) * _transformMatrix;
    _transformMatrix = glm::translate(_transformMatrix, -_position);
}

}

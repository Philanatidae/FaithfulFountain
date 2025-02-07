/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/Camera.hpp>

#include <ff/Locator.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ff {
    Camera::Camera(const bool& generateUniformBuffer)
        :_combinedMatrix(glm::identity<glm::mat4>()),
        _combinedInverse(glm::identity<glm::mat4>()) {
        if(generateUniformBuffer) {
            _uniforms = std::make_unique<CameraUniforms>();
            _uniformBuffer = Locator::getGraphicsDevice().createUniformBuffer<CameraUniforms>();
        }
    }

    glm::mat4x4 Camera::getCombined(const double& alpha) const {
        // @todo Camera alpha
        return _combinedMatrix;
    }
    const glm::mat4x4& Camera::getCombinedInverse() const {
        return _combinedInverse;
    }

    float Camera::getClearDepth() const {
        return isGreaterZDepthIncrease() ? 1 : 0;
    }
    DepthCompareFunction Camera::getDepthCompareFunction() const {
        return isGreaterZDepthIncrease() ? DepthCompareFunction::GREATER : DepthCompareFunction::LESS;
    }

    const std::shared_ptr<ff::UniformBuffer<CameraUniforms>>& Camera::getUniformBuffer() const {
        return _uniformBuffer;
    }

    void Camera::updateCombinedMatrix(const glm::mat4x4 newCombined) {
        _combinedMatrix = newCombined;
        _combinedInverse = glm::inverse(_combinedMatrix);
        
        if(_uniforms != nullptr) {
            _uniforms->combined = _combinedMatrix;
            // @todo Should we wait until the game is rendering to update this? Via Event?
            _uniformBuffer->bufferData(_uniforms.get());
        }
    }
}

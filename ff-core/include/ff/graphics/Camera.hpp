/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_CAMERA_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_CAMERA_HPP

#include <glm/glm.hpp>

#include <memory>
#include <ff/graphics/UniformBuffer.hpp>
#include <ff/graphics/IDepthStencilState.hpp>

namespace ff {
    struct CameraUniforms {
        glm::mat4x4 combined;
    };

    class Camera {
    public:
        Camera(const bool& generateUniformBuffer = true);
        virtual ~Camera() = default;

        glm::mat4x4 getCombined(const double& alpha = 0) const;
        const glm::mat4x4& getCombinedInverse() const;

        virtual glm::mat4x4 getTransformMatrix() const = 0;
        virtual glm::mat4x4 getProjectionMatrix() const = 0;

        virtual bool isGreaterZDepthIncrease() const = 0;

        float getClearDepth() const;
        DepthCompareFunction getDepthCompareFunction() const;

        const std::shared_ptr<ff::UniformBuffer<CameraUniforms>>& getUniformBuffer() const;

    protected:
        void updateCombinedMatrix(const glm::mat4x4 newCombined);

    private:
        glm::mat4x4 _combinedMatrix;
        glm::mat4x4 _combinedInverse;
        std::unique_ptr<ff::CameraUniforms> _uniforms;
        std::shared_ptr<ff::UniformBuffer<CameraUniforms>> _uniformBuffer;
    };
}

#endif

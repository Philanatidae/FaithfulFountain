/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_ORTHOGRAPHIC_CAMERA_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_ORTHOGRAPHIC_CAMERA_HPP

#include <ff/graphics/Camera.hpp>
#include <glm/glm.hpp>

#include <ff/events/ResizeEvent.hpp>
#include <ff/messages/EventListener.hpp>

#include <ff/math/Rectangle.hpp>

namespace ff {
    class OrthographicCamera : public Camera {
    public:
        OrthographicCamera(const float& width, const float& height,
            const bool& generateUniformBuffer = true);
        OrthographicCamera(float const& left,
            float const& right,
            float const& top,
            float const& bottom,
            const bool& generateUniformBuffer);
        virtual ~OrthographicCamera();

        glm::mat4x4 getTransformMatrix() const override;
        glm::mat4x4 getProjectionMatrix() const override;
        Rectangle getRenderArea() const;
        Rectangle getSafeArea() const;

        bool isGreaterZDepthIncrease() const override;

        const glm::vec2& getPosition() const;
        void setPosition(const glm::vec2& position);
        const float& getRotation() const;
        void setRotation(const float& rotation);
        const float& getZoom() const;
        void setZoom(const float& zoom);

        void adjust(const float& width, const float& height, const bool& noSafeArea = false);
        void adjust(const float& left, const float& right,
            const float& top, const float& bottom, const bool& noSafeArea = false);
        void update();

    private:
        glm::mat4x4 _projectionMatrix;
        glm::mat4x4 _transformMatrix;
        glm::vec2 _position;
        float _rotation;
        float _zoom;

        Rectangle _renderArea;
        Rectangle _safeArea;

        void updateSafeAreaFromRenderArea();
    };
}

#endif

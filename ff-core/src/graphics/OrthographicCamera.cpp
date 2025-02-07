/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ff/graphics/OrthographicCamera.hpp>

#include <ff/CVars.hpp>

#include <ff/util/Macros.hpp>

#include <ff/Locator.hpp>

namespace ff {
    OrthographicCamera::OrthographicCamera(const float& width, const float& height,
        const bool& generateUniformBuffer)
        :Camera(generateUniformBuffer),
            _projectionMatrix(1.0f),
            _transformMatrix(glm::mat4x4(1.0f)),
            _position(0),
            _rotation(0),
            _zoom(1.0f),
        _renderArea(0, 0),
        _safeArea(0, 0) {
        adjust(width, height);
        update();
    }
    OrthographicCamera::OrthographicCamera(float const& left,
        float const& right,
        float const& top,
        float const& bottom,
        const bool& generateUniformBuffer)
        :Camera(generateUniformBuffer),
            _projectionMatrix(1.0f),
            _transformMatrix(glm::mat4x4(1.0f)),
            _position(0),
            _rotation(0),
            _zoom(1.0f),
        _renderArea(0, 0),
        _safeArea(0, 0) {
        adjust(left,
            right,
            top,
            bottom,
            true);
        update();
    }
    OrthographicCamera::~OrthographicCamera() {
    }

    glm::mat4x4 OrthographicCamera::getTransformMatrix() const {
        return _transformMatrix;
    }
    glm::mat4x4 OrthographicCamera::getProjectionMatrix() const {
        return _projectionMatrix;
    }
    Rectangle OrthographicCamera::getRenderArea() const {
        return _renderArea;
    }
    Rectangle OrthographicCamera::getSafeArea() const {
        return _safeArea;
    }
    bool OrthographicCamera::isGreaterZDepthIncrease() const {
        return true;
    }

    const glm::vec2& OrthographicCamera::getPosition() const {
        return _position;
    }
    void OrthographicCamera::setPosition(const glm::vec2& position) {
        _position = position;
        update();
    }
    const float& OrthographicCamera::getRotation() const {
        return _rotation;
    }
    void OrthographicCamera::setRotation(const float& rotation) {
        _rotation = rotation;
        update();
    }
    const float& OrthographicCamera::getZoom() const {
        return _zoom;
    }
    void OrthographicCamera::setZoom(const float& zoom) {
        _zoom = zoom;
        update();
    }

    void OrthographicCamera::adjust(const float& width, const float& height, const bool& noSafeArea) {
        _projectionMatrix = glm::ortho(-width / 2.0f,
            width / 2.0f,
            -height / 2.0f,
            height / 2.0f);
        _renderArea = Rectangle(width, height);
        if(noSafeArea) {
            _safeArea = _renderArea;
        } else {
            updateSafeAreaFromRenderArea();
        }
    }
    void OrthographicCamera::adjust(const float& left, const float& right,
        const float& top, const float& bottom, const bool& noSafeArea) {
        _projectionMatrix = glm::ortho(left,
            right,
            bottom,
            top);
        const float width = right - left;
        const float height = top - bottom;
        _renderArea = Rectangle(left + width / 2,
            bottom + height / 2,
            width,
            height);
        if(noSafeArea) {
            _safeArea = _renderArea;
        } else {
            updateSafeAreaFromRenderArea();
        }
    }
    void OrthographicCamera::update() {
        _transformMatrix = glm::mat4x4(1.0f);
        _transformMatrix = glm::rotate(_transformMatrix, getRotation(), glm::vec3(0, 0, 1));
        _transformMatrix = glm::scale(_transformMatrix, glm::vec3(getZoom(), getZoom(), 1.0f));
        _transformMatrix = glm::translate(_transformMatrix, glm::vec3(-getPosition(), 0));

        updateCombinedMatrix(_projectionMatrix * getTransformMatrix());
    }

    void OrthographicCamera::updateSafeAreaFromRenderArea() {
        glm::vec2 renderAreaCenter = getRenderArea().getCenter();

        _safeArea = Rectangle(renderAreaCenter.x, renderAreaCenter.y,
            getRenderArea().getWidth() * (1 - CVars::get<float>("view_safe_area_padding_left") - CVars::get<float>("view_safe_area_padding_right")),
            getRenderArea().getHeight() * (1 - CVars::get<float>("view_safe_area_padding_top") - CVars::get<float>("view_safe_area_padding_bottom")));
    }
}

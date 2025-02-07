/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/math/Circle.hpp>

namespace ff {
    Circle::Circle()
        :Circle(0) {
    }
    Circle::Circle(const glm::vec2& position, const float& radius)
        :_position(position),_radius(radius),_radiusSquared(radius * radius) {
    }
    Circle::Circle(const float& radius)
        :Circle(glm::vec2(0, 0), radius) {
    }
    Circle::Circle(const Circle& other)
        :_position(other._position),_radius(other._radius),
        _radiusSquared(other._radiusSquared) {
    }
    Circle::~Circle() {
    }

    const float& Circle::getRadius() const {
        return _radius;
    }
    void Circle::setRadius(const float& radius) {
        _radius = radius;
        _radiusSquared = radius * radius;
    }
    const float& Circle::getRadiusSquared() const {
        return _radiusSquared;
    }
    void Circle::setRadiusSquared(const float& radiusSquared) {
        _radiusSquared = radiusSquared;
        _radius = glm::sqrt(radiusSquared);
    }

    const glm::vec2& Circle::getPosition() const {
        return _position;
    }
    void Circle::setPosition(const glm::vec2& position) {
        _position = position;
    }

    bool Circle::contains(const glm::vec2& point) {
        return glm::distance(point, _position) < _radius;
    }
    bool Circle::intersects(const Circle& other) {
        return glm::distance(other._position, _position) < other._radius + _radius;
    }
}

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/math/Rectangle.hpp>

namespace ff {
    Rectangle::Rectangle(const glm::vec2& bottomLeft, const glm::vec2& topRight)
        :_bottomLeft(bottomLeft),_topRight(topRight) {
    }
    Rectangle::Rectangle(const float& width, const float& height)
        :Rectangle(0, 0, width, height) {
    }
    Rectangle::Rectangle(const glm::vec2& bounds)
        :Rectangle(bounds.x, bounds.y) {
    }
    Rectangle::Rectangle(const float& x, const float& y, const float& width, const float& height)
        :Rectangle(glm::vec2(x - width / 2, y - height / 2), glm::vec2(x + width / 2, y + height / 2)) {
    }
    Rectangle::Rectangle(const Rectangle& other)
        :Rectangle(other._bottomLeft, other._topRight) {
    }
    Rectangle::~Rectangle() {
    }

    float Rectangle::getWidth() const {
        return _topRight.x - _bottomLeft.x;
    }
    float Rectangle::getHeight() const {
        return _topRight.y - _bottomLeft.y;
    }

    const glm::vec2& Rectangle::getBottomLeft() const {
        return _bottomLeft;
    }
    const glm::vec2& Rectangle::getTopRight() const {
        return _topRight;
    }
    glm::vec2 Rectangle::getCenter() const {
        return glm::vec2((_topRight.x - _bottomLeft.x) / 2 + _bottomLeft.x,
            (_topRight.y - _bottomLeft.y) / 2 + _bottomLeft.y);
    }

    const glm::vec2 Rectangle::getPosition() const {
        return (_topRight - _bottomLeft) / 2.0f + _bottomLeft;
    }

    bool Rectangle::intersects(const Rectangle& other) {
        return other._bottomLeft.x <= _topRight.x
            && other._topRight.x >= _bottomLeft.x
            && other._bottomLeft.y <= _topRight.y
            && other._topRight.y >= _bottomLeft.y;
    }
    bool Rectangle::contains(const Rectangle& other) {
        return other._bottomLeft.x >= _bottomLeft.x
            && other._topRight.x <= _topRight.x
            && other._bottomLeft.y >= _bottomLeft.y
            && other._topRight.y <= _topRight.y;
    }
    bool Rectangle::contains(const glm::vec2& point) {
        return point.x >= _bottomLeft.x
            && point.x <= _topRight.x
            && point.y >= _bottomLeft.y
            && point.y <= _topRight.y;
    }
}

ff::Rectangle operator+(const ff::Rectangle& lhs, const glm::vec2& rhs) {
    return ff::Rectangle(lhs.getPosition().x + rhs.x, lhs.getPosition().y + rhs.y,
        lhs.getWidth(), lhs.getHeight());
}

ff::Rectangle operator*(const ff::Rectangle& lhs, const float& rhs) {
    return ff::Rectangle(lhs.getPosition().x, lhs.getPosition().y,
        lhs.getWidth() * rhs, lhs.getHeight() * rhs);
}

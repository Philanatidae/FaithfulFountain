/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MATH_RECTANGLE_HPP
#define _FAITHFUL_FOUNTAIN_MATH_RECTANGLE_HPP

#include <glm/glm.hpp>

namespace ff {
    class Rectangle {
    public:
        explicit Rectangle(const glm::vec2& bottomLeft, const glm::vec2& topRight);
        explicit Rectangle(const float& width, const float& height);
        explicit Rectangle(const glm::vec2& bounds);
        explicit Rectangle(const float& x, const float& y, const float& width, const float& height);
        Rectangle(const Rectangle& other);
        virtual ~Rectangle();

        float getWidth() const;
        float getHeight() const;

        const glm::vec2& getBottomLeft() const;
        const glm::vec2& getTopRight() const;
        glm::vec2 getCenter() const;

        const glm::vec2 getPosition() const;

        bool intersects(const Rectangle& other);
        bool contains(const Rectangle& other);
        bool contains(const glm::vec2& point);

    private:
        glm::vec2 _bottomLeft;
        glm::vec2 _topRight;
    };

}

ff::Rectangle operator+(const ff::Rectangle& lhs, const glm::vec2& rhs);
inline ff::Rectangle operator+(const glm::vec2& lhs, const ff::Rectangle& rhs) {
    return rhs + lhs;
}

ff::Rectangle operator*(const ff::Rectangle& lhs, const float& rhs);
inline ff::Rectangle operator*(const float& lhs, const ff::Rectangle& rhs) {
    return rhs * lhs;
}
inline ff::Rectangle operator*(const ff::Rectangle& lhs, const int& rhs) {
    return lhs * (float)rhs;
}
inline ff::Rectangle operator*(const int& lhs, const ff::Rectangle& rhs) {
    return rhs * (float)lhs;
}

#endif

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MATH_CIRCLE_HPP
#define _FAITHFUL_FOUNTAIN_MATH_CIRCLE_HPP

#include <glm/glm.hpp>

namespace ff {
    class Circle {
    public:
        Circle();
        Circle(const glm::vec2& position, const float& radius);
        Circle(const float& radius);
        Circle(const Circle& other);
        virtual ~Circle();

        const float& getRadius() const;
        void setRadius(const float& radius);
        const float& getRadiusSquared() const;
        void setRadiusSquared(const float& radiusSquared);

        const glm::vec2& getPosition() const;
        void setPosition(const glm::vec2& position);

        bool contains(const glm::vec2& point);
        bool intersects(const Circle& other);

    private:
        glm::vec2 _position;
        float _radius;
        float _radiusSquared;
    };
}

#endif

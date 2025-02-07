/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/components/Collision2DComponent.hpp>

#include <ff/math/VectorHelper.hpp>
#include <limits>

namespace ff {
    Collision2DShape::Collision2DShape()
        :_offset(glm::vec2(0)) {}
    Collision2DShape::~Collision2DShape() {}

    const glm::vec2& Collision2DShape::getOffset() const {
        return _offset;
    }
    void Collision2DShape::setOffset(const glm::vec2& offset) {
        _offset = offset;
    }


    CircleCollision2DShape::CircleCollision2DShape(const float& radius)
        :_radius(radius),_radiusSquared(radius * radius) {
    }
    CircleCollision2DShape::~CircleCollision2DShape() {
    }

    const float& CircleCollision2DShape::getRadius() const {
        return _radius;
    }
    void CircleCollision2DShape::setRadius(const float& radius) {
        _radius = radius;
        _radiusSquared = radius * radius;
    }
    const float& CircleCollision2DShape::getRadiusSquared() const {
        return _radiusSquared;
    }
    void CircleCollision2DShape::setRadiusSquared(const float& radiusSquared) {
        _radiusSquared = radiusSquared;
        _radius = glm::sqrt(radiusSquared);
    }

    Rectangle CircleCollision2DShape::getAABB(const float& cos, const float& sin, const float& scale, const glm::vec2& position) {
        glm::vec2 tOffset = VectorHelper::rotate(getOffset(), cos, sin) * scale;
        Rectangle rect(tOffset.x + position.x, tOffset.y + position.y,
            2 * _radius * scale, 2 * _radius * scale);
        return rect;
    }

    Collision2DShape* CircleCollision2DShape::createRawPtrCopy() {
        return new CircleCollision2DShape(*this);
    }


    PolygonCollision2DShape::PolygonCollision2DShape(const std::vector<glm::vec2>& vertices) {
        // Detect cw/ccw
        // https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
        float sum = 0;
        for(size_t i = 0; i < vertices.size(); i++) {
            glm::vec2 v1 = vertices[i];
            glm::vec2 v2 = vertices[(i + 1) % vertices.size()];

            sum += (v2.x - v1.x) * (v2.y + v1.y);
        }
        if(sum <= 0) {
            // Already in ccw
            _vertices = vertices;
        } else {
            // cw: we need to reverse the rotation
            _vertices.resize(vertices.size());
            for(size_t i = 0; i < vertices.size(); i++) {
                _vertices[i] = vertices[i];
            }
        }

        // Compute normals
        _normals.resize(vertices.size());
        for(size_t i = 0; i < vertices.size(); i++) {
            glm::vec2 v1 = vertices[i];
            glm::vec2 v2 = vertices[(i + 1) % vertices.size()];

            _normals[i] = -VectorHelper::normal(v2 - v1); // Normal is right-handed, we need left-handed
        }
    }
    PolygonCollision2DShape::~PolygonCollision2DShape() {
    }

    const std::vector<glm::vec2>& PolygonCollision2DShape::getVertices() const {
        return _vertices;
    }
    const std::vector<glm::vec2>& PolygonCollision2DShape::getNormals() const {
        return _normals;
    }

    Rectangle PolygonCollision2DShape::getAABB(const float& cos, const float& sin, const float& scale, const glm::vec2& position) {
        glm::vec2 bottomLeft(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
            topRight(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

        for(size_t i = 0; i < _vertices.size(); i++) {
            glm::vec2 tVertex = VectorHelper::rotate(_vertices[i] + getOffset(), cos, sin) * scale;

            if(tVertex.x < bottomLeft.x) {
                bottomLeft.x = tVertex.x;
            }
            if(tVertex.x > topRight.x) {
                topRight.x = tVertex.x;
            }
            if(tVertex.y < bottomLeft.y) {
                bottomLeft.y = tVertex.y;
            }
            if(tVertex.y > topRight.y) {
                topRight.y = tVertex.y;
            }
        }

        return Rectangle(bottomLeft + position, topRight + position);
    }

    Collision2DShape* PolygonCollision2DShape::createRawPtrCopy() {
        return new PolygonCollision2DShape(*this);
    }


    Collision2DComponent::Collision2DComponent(const std::initializer_list<Collision2DShape*>& shapes,
            const CollisionGroup& group,
            const CollisionGroup& mask)
        :shapes(shapes),group(group),mask(mask) {
    }
    Collision2DComponent::Collision2DComponent(const Collision2DComponent& other)
        :group(other.group),mask(other.mask) {
        for(auto it = other.shapes.begin();
            it != other.shapes.end();
            it++) {
            shapes.emplace_back((*it)->createRawPtrCopy());
        }
    }
    Collision2DComponent::~Collision2DComponent() {
        for(auto it = shapes.begin();
            it != shapes.end();
            it++) {
            Collision2DShape* shapePtr = *it;
            delete shapePtr;
        }
    }

    size_t Collision2DComponent::getCollidingCount() const {
        return _currentlyCollidingWith.size();
    }
}

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMPONENTS_COLLISION_2D_COMPONENT_HPP
#define _FAITHFUL_FOUNTAIN_COMPONENTS_COLLISION_2D_COMPONENT_HPP

#include <ff/actors/Component.hpp>
#include <ff/actors/Actor.hpp>
#include <unordered_set>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <ff/math/Rectangle.hpp>

namespace ff {
    typedef uint8_t CollisionGroup;

    constexpr CollisionGroup COLLISION_GROUP_ONE = 0x1;
    constexpr CollisionGroup COLLISION_GROUP_TWO = 0x2;
    constexpr CollisionGroup COLLISION_GROUP_THREE = 0x4;
    constexpr CollisionGroup COLLISION_GROUP_FOUR = 0x8;
    constexpr CollisionGroup COLLISION_GROUP_FIVE = 0x10;
    constexpr CollisionGroup COLLISION_GROUP_SIX = 0x20;
    constexpr CollisionGroup COLLISION_GROUP_SEVEN = 0x40;
    constexpr CollisionGroup COLLISION_GROUP_EIGHT = 0x80;

    constexpr CollisionGroup COLLISION_GROUP_ALL = 0xFF;

    class Collision2DShape {
    friend struct Collision2DComponent;

    public:
        Collision2DShape();
        virtual ~Collision2DShape();

        virtual Rectangle getAABB(const float& cos, const float& sin, const float& scale, const glm::vec2& position) = 0;

        const glm::vec2& getOffset() const;
        void setOffset(const glm::vec2& offset);

    protected:
        virtual Collision2DShape* createRawPtrCopy() = 0;

    private:
        glm::vec2 _offset;
    };

    class CircleCollision2DShape : public Collision2DShape {
    public:
        CircleCollision2DShape(const float& radius);
        ~CircleCollision2DShape();

        const float& getRadius() const;
        void setRadius(const float& radius);
        const float& getRadiusSquared() const;
        void setRadiusSquared(const float& radiusSquared);

        Rectangle getAABB(const float& cos, const float& sin, const float& scale, const glm::vec2& position) override;

    protected:
        Collision2DShape* createRawPtrCopy() override;

    private:
        float _radius;
        float _radiusSquared;
    };

    class PolygonCollision2DShape : public Collision2DShape {
    public:
        PolygonCollision2DShape(const std::vector<glm::vec2>& vertices);
        ~PolygonCollision2DShape();

        const std::vector<glm::vec2>& getVertices() const;
        const std::vector<glm::vec2>& getNormals() const;

        Rectangle getAABB(const float& cos, const float& sin, const float& scale, const glm::vec2& position) override;

    protected:
        Collision2DShape* createRawPtrCopy() override;

    private:
        std::vector<glm::vec2> _vertices;
        std::vector<glm::vec2> _normals;
    };

    struct Collision2DComponent final : public Component<Collision2DComponent> {
        friend class Collision2DDetectionSystem;

        Collision2DComponent(const std::initializer_list<Collision2DShape*>& shapes = {},
            const CollisionGroup& group = COLLISION_GROUP_ONE,
            const CollisionGroup& mask = COLLISION_GROUP_ALL);
        Collision2DComponent(const Collision2DComponent& other);
        ~Collision2DComponent();

        std::vector<Collision2DShape*> shapes;

        CollisionGroup group;
        CollisionGroup mask;

        size_t getCollidingCount() const;
    private:
        std::unordered_set<Actor_t> _currentlyCollidingWith;
    };
}

#endif

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SYSTEMS_COLLISION2DDETECTIONSYSTEM_HPP
#define _FAITHFUL_FOUNTAIN_SYSTEMS_COLLISION2DDETECTIONSYSTEM_HPP

#include <ff/actors/ActorManager.hpp>
#include <ff/processes/Process.hpp>
#include <glm/glm.hpp>
#include <ff/components/Collision2DComponent.hpp>

namespace ff {
    class Collision2DDetectionSystem final : public Process {
    public:
        Collision2DDetectionSystem(ActorManager* const& actorManagerPtr, ProcessPriority_t const& priority = ff::ProcessPriority::DEFAULT);
        virtual ~Collision2DDetectionSystem();

    protected:
        void onUpdate(const float& dt) override;

    private:
        ActorManager* const _actorManagerPtr;

        bool checkShapeIntersection(const glm::vec2& posA, const float& cosA, const float& sinA, const float& scaleA, const Collision2DShape& shapeA, glm::vec2& normA,
            const glm::vec2& posB, const float& cosB, const float& sinB, const float& scaleB, const Collision2DShape& shapeB, glm::vec2& normB, float& penetration);
        bool checkCircleCircleIntersection(const glm::vec2& posA, const CircleCollision2DShape& circleA, const float& scaleA, glm::vec2& normA,
            const glm::vec2& posB, const CircleCollision2DShape& circleB, const float& scaleB, glm::vec2& normB, float& penetration);
        bool checkPolygonCircleIntersection(const glm::vec2& posA, const float& cosA, const float& sinA, const float& scaleA, const PolygonCollision2DShape& polygonA, glm::vec2& normA,
            const glm::vec2& posB, const CircleCollision2DShape& circleB, const float& scaleB, glm::vec2& normBm, float& penetration);
        bool checkPolygonPolygonIntersection(const glm::vec2& posA, const float& cosA, const float& sinA, const float& scaleA, const PolygonCollision2DShape& polygonA, glm::vec2& normA,
            const glm::vec2& posB, const float& cosB, const float& sinB, const float& scaleB, const PolygonCollision2DShape& polygonB, glm::vec2& normB, float& penetration);

        glm::vec2 computeSupport(const std::vector<glm::vec2>& verts, const glm::vec2& dir);
        float findAxisOfLeastPenetration(size_t& faceIndex, const std::vector<glm::vec2>& vertsA, const std::vector<glm::vec2>& vertsB);
    };
}

#endif

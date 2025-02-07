/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/systems/Collision2DDetectionSystem.hpp>

#include <ff/components/TransformComponent.hpp>

#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <ff/Console.hpp>
#include <ff/math/VectorHelper.hpp>
#include <ff/math/Rectangle.hpp>
#include <ff/math/Circle.hpp>

#include <ff/messages/MessageBus.hpp>
#include <ff/events/collision/CollisionStartEvent.hpp>
#include <ff/events/collision/CollisionEndEvent.hpp>

#include <ff/components/StaticColliderComponent.hpp>
#include <ff/components/KinematicColliderComponent.hpp>
#include <ff/components/DynamicColliderComponent.hpp>

#include <chrono>
#include <algorithm>

#include <ff/util/Macros.hpp>

namespace ff {
    Collision2DDetectionSystem::Collision2DDetectionSystem(ActorManager* const& actorManagerPtr, ProcessPriority_t const& priority)
        :_actorManagerPtr(actorManagerPtr) {
        FF_UNUSED(priority);
    }
    Collision2DDetectionSystem::~Collision2DDetectionSystem() {
    }

    void Collision2DDetectionSystem::onUpdate(const float& dt) {
        FF_CONSOLE_WARN("2D collision detection system is not updated to support multi-axis stretch.");

        FF_UNUSED(dt);

        std::unordered_map<Actor_t, std::unordered_set<Actor_t>> processedPairs; // TODO: This can be achieved with pairs (or specialized struct), probably will be faster
        _actorManagerPtr->getActorsFor(Family::all<TransformComponent, Collision2DComponent, DynamicColliderComponent>().get())
            .each([this, &processedPairs](Actor_t actorA) {
            auto& transformCompA = _actorManagerPtr->getComponent<TransformComponent>(actorA);
            auto& collisionCompA = _actorManagerPtr->getComponent<Collision2DComponent>(actorA);

            float cosA = glm::cos(transformCompA.get2DRotation()),
                sinA = glm::sin(transformCompA.get2DRotation());

            _actorManagerPtr->getActorsFor(Family::all<TransformComponent, Collision2DComponent>().one<StaticColliderComponent, KinematicColliderComponent>().get())
                .each([this, &processedPairs, actorA, &transformCompA, &collisionCompA, cosA, sinA](Actor_t actorB) {
                auto& transformCompB = _actorManagerPtr->getComponent<TransformComponent>(actorB);
                auto& collisionCompB = _actorManagerPtr->getComponent<Collision2DComponent>(actorB);

                if(actorA == actorB) {
                    return;
                }
                if(processedPairs[actorA].find(actorB) != processedPairs[actorA].end()) {
                    return;
                }
                if(processedPairs[actorB].find(actorA) != processedPairs[actorB].end()) {
                    return;
                }

                // Add it to entityB's list because A->B won't occur again (B->A might)
                processedPairs[actorB].insert(actorA);

                if((collisionCompA.mask & collisionCompB.group) == 0
                    && (collisionCompB.mask & collisionCompA.group) == 0) {
                    return;
                }

                float cosB = glm::cos(transformCompB.get2DRotation()),
                    sinB = glm::sin(transformCompB.get2DRotation());

                bool resolved = false; // If true, _any_ of the shapes have intersected
                bool intersecting = false;
                glm::vec2 normA;
                glm::vec2 normB;
                float penetration;
                for(auto shapeItA = collisionCompA.shapes.begin();
                    shapeItA != collisionCompA.shapes.end();
                    shapeItA++) {
                    Rectangle AABB_A = (*shapeItA)->getAABB(cosA, sinA, transformCompA.getScale(), transformCompA.getPosition());

                    for(auto shapeItB = collisionCompB.shapes.begin();
                    shapeItB != collisionCompB.shapes.end();
                    shapeItB++) {
                        Rectangle AABB_B = (*shapeItB)->getAABB(cosB, sinB, transformCompB.getScale(), transformCompB.getPosition());

                        if(AABB_B.intersects(AABB_A)) {
                            // _May_ be intersecting
                            glm::vec2 pA = transformCompA.get2DPosition()
                                + VectorHelper::rotate((*shapeItA)->getOffset(), cosA, sinA) * transformCompA.getScale();
                            glm::vec2 pB = transformCompB.get2DPosition()
                                + VectorHelper::rotate((*shapeItB)->getOffset(), cosB, sinB) * transformCompB.getScale();
                            if(checkShapeIntersection(pA, cosA, sinA, transformCompA.getScale(), **shapeItA, normA,
                                pB, cosB, sinB, transformCompB.getScale(), **shapeItB, normB, penetration)) {
                                intersecting = true;
                                resolved = true;
                                break;
                            }
                        } else {
                            // Guaranteed to not be intersecting (this shape)
                            continue;
                        }
                    }

                    if(resolved) {
                        break;
                    }
                }

                bool previouslyIntersecting = collisionCompA._currentlyCollidingWith.find(actorB) != collisionCompA._currentlyCollidingWith.end()
                    || collisionCompB._currentlyCollidingWith.find(actorA) != collisionCompB._currentlyCollidingWith.end();
                if(!previouslyIntersecting && intersecting) {
                    ff::Locator::getMessageBus().dispatch<CollisionStartEvent>(actorA, normA, actorB, normB, penetration);
                    collisionCompA._currentlyCollidingWith.insert(actorB);
                    collisionCompB._currentlyCollidingWith.insert(actorA);
                }
                if(previouslyIntersecting && !intersecting) {
                    ff::Locator::getMessageBus().dispatch<CollisionEndEvent>(actorA, actorB);
                    collisionCompA._currentlyCollidingWith.erase(actorB);
                    collisionCompB._currentlyCollidingWith.erase(actorA);
                }
            });
        });
    }

    bool Collision2DDetectionSystem::checkShapeIntersection(const glm::vec2& posA, const float& cosA, const float& sinA, const float& scaleA, const Collision2DShape& shapeA, glm::vec2& normA,
        const glm::vec2& posB, const float& cosB, const float& sinB, const float& scaleB, const Collision2DShape& shapeB, glm::vec2& normB, float& penetration) {
        CircleCollision2DShape const* circleA = dynamic_cast<CircleCollision2DShape const*>(&shapeA);
        PolygonCollision2DShape const* polygonA = dynamic_cast<PolygonCollision2DShape const*>(&shapeA);

        CircleCollision2DShape const* circleB = dynamic_cast<CircleCollision2DShape const*>(&shapeB);
        PolygonCollision2DShape const* polygonB = dynamic_cast<PolygonCollision2DShape const*>(&shapeB);

        // TODO: Get rid of dynamic cast and use enum/int instead

        if(circleA != nullptr && circleB != nullptr) {
            return checkCircleCircleIntersection(posA, *circleA, scaleA, normA,
                posB, *circleB, scaleB, normB, penetration);
        }

        if(circleB != nullptr && polygonA != nullptr) {
            return checkPolygonCircleIntersection(posA, cosA, sinA, scaleA, *polygonA, normA,
                posB, *circleB, scaleB, normB, penetration);
        }
        if(circleA != nullptr && polygonB != nullptr) {
            return checkPolygonCircleIntersection(posB, cosB, sinB, scaleB, *polygonB, normB,
                posA, *circleA, scaleA, normA, penetration);
        }

        if(polygonA != nullptr && polygonB != nullptr) {
            return checkPolygonPolygonIntersection(posA, cosA, sinA, scaleA, *polygonA, normA,
                posB, cosB, sinB, scaleB, *polygonB, normB, penetration);
        }

        FF_CONSOLE_ERROR("Unknown 2D collision case.");

        return false;
    }
    bool Collision2DDetectionSystem::checkCircleCircleIntersection(const glm::vec2& posA, const CircleCollision2DShape& circleA, const float& scaleA, glm::vec2& normA,
        const glm::vec2& posB, const CircleCollision2DShape& circleB, const float& scaleB, glm::vec2& normB, float& penetration) {
        float const distance = glm::distance(posB, posA);
        bool colliding = distance < circleB.getRadius() * scaleB + circleA.getRadius() * scaleA;
        if(colliding) {
            if(distance > 0) {
                normA = glm::normalize(posB - posA);
                normB = -normA;
            } else {
                normA = normB = glm::vec2(0, 0);
            }
            penetration = (circleA.getRadius() * scaleA + circleB.getRadius() * scaleB) - distance;
        }
        return colliding;
    }
    bool Collision2DDetectionSystem::checkPolygonCircleIntersection(const glm::vec2& posA, const float& cosA, const float& sinA, const float& scaleA, const PolygonCollision2DShape& polygonA, glm::vec2& normA,
        const glm::vec2& posB, const CircleCollision2DShape& circleB, const float& scaleB, glm::vec2& normB, float& penetration) {
        // @todo This case fails if the circle is fully enclosed by the polygon.
        // Rare, but possible.

        std::vector<glm::vec2> worldPolyA;
        worldPolyA.resize(polygonA.getVertices().size());
        for(size_t i = 0; i < polygonA.getVertices().size(); i++) {
            worldPolyA[i] = VectorHelper::rotate(polygonA.getVertices()[i], cosA, sinA) * scaleA + posA;
        }

        Circle worldCircleB(posB, circleB.getRadius() * scaleB);

        glm::vec2 closestV1, closestV2;
        float closestDistanceToCircle = -1;
        glm::vec2 closestVert;
        float closestVertDistance = -1;
        for(size_t i = 0; i < worldPolyA.size(); i++) {
            glm::vec2& v2 = worldPolyA[i];

            // Closest vertex
            float vertDistanceToCircle = glm::distance(worldCircleB.getPosition(), v2);
            if(closestVertDistance < 0
                || vertDistanceToCircle < closestVertDistance) {
                closestVertDistance = vertDistanceToCircle;
                closestVert = v2;
            }

            // Closest segment
            size_t j = (i + 1) % worldPolyA.size();

            glm::vec2& v1 = worldPolyA[j];

            // Distance from circle center to segment
            float circleOnSegment = glm::dot(worldCircleB.getPosition() - v1, glm::normalize(v2 - v1));
            float hyp = glm::distance(worldCircleB.getPosition(), v1);
            float circleToSegment = glm::sqrt(hyp * hyp - circleOnSegment * circleOnSegment);

            if(circleOnSegment > 0 && circleOnSegment < glm::distance(v2, v1)) { // If distance is on segment
                if(closestDistanceToCircle < 0 // First loop
                    || circleToSegment < closestDistanceToCircle) {
                    closestV1 = v1;
                    closestV2 = v2;
                    closestDistanceToCircle = circleToSegment;
                }
            }
        }
        FF_ASSERT(closestDistanceToCircle >= 0 || closestVertDistance >= 0, "Circle-polygon closest distance to segment/vertex algorithm failed.");
        if(closestDistanceToCircle < 0) {
            if(worldCircleB.contains(closestVert)) {
                normA = glm::normalize(posB - closestVert);
                normB = -normA;
                penetration = glm::distance(posB, closestVert);
                return true;
            }
            // Since we couldn't find a closest segment, we must not be touching the polygon
            return false;
        }

        // We have the closest segment of the polygon.
        // Next, find which Voronoi region the circle is in.

        // Cast circle position onto segment
        glm::vec2 segment = closestV2 - closestV1;
        glm::vec2 segmentNormalized = glm::normalize(segment);
        float circleOnSegment = glm::dot(segmentNormalized, worldCircleB.getPosition() - closestV1);

        // If the circle is along the segment, it's on the face
        // region and we can get the normal.
        if(circleOnSegment > 0
            || circleOnSegment < glm::length(segment)) {
            // Find point along segment closest to the circle (perpendicular from segment to circle center)
            glm::vec2 intersectionPoint = segmentNormalized * circleOnSegment + closestV1;
            normA = glm::normalize(posB - intersectionPoint);
            normB = -normA;;
            penetration = glm::distance(posB, intersectionPoint);
            return true;
        }

        bool containsEndpoint = false;
        glm::vec2 endpoint;
        if(worldCircleB.contains(closestV1)) {
            FF_ASSERT(!containsEndpoint, "Circle contains two points of the polygon; this is not handled yet.");
            containsEndpoint = true;
            endpoint = closestV1;
        }
        if(worldCircleB.contains(closestV2)) {
            FF_ASSERT(!containsEndpoint, "Circle contains two points of the polygon; this is not handled yet.");
            containsEndpoint = true;
            endpoint = closestV2;
        }
        if(containsEndpoint) {
            penetration = glm::distance(posB, endpoint);
            if(penetration > 0) {
                normA = glm::normalize(posB - endpoint);
                normB = -normA;
            } else {
                normA = normB = glm::vec2(0, 0);
            }

            return true;
        }

        return false;
    }
    bool Collision2DDetectionSystem::checkPolygonPolygonIntersection(const glm::vec2& posA, const float& cosA, const float& sinA, const float& scaleA, const PolygonCollision2DShape& polygonA, glm::vec2& normA,
        const glm::vec2& posB, const float& cosB, const float& sinB, const float& scaleB, const PolygonCollision2DShape& polygonB, glm::vec2& normB, float& penetration) {

        std::vector<glm::vec2> worldPolyA;
        worldPolyA.resize(polygonA.getVertices().size());
        for(size_t i = 0; i < polygonA.getVertices().size(); i++) {
            worldPolyA[i] = VectorHelper::rotate(polygonA.getVertices()[i], cosA, sinA) * scaleA + posA;
        }

        std::vector<glm::vec2> worldPolyB;
        worldPolyB.resize(polygonB.getVertices().size());
        for(size_t i = 0; i < polygonB.getVertices().size(); i++) {
            worldPolyB[i] = VectorHelper::rotate(polygonB.getVertices()[i], cosB, sinB) * scaleB + posB;
        }

        size_t indexA;
        float penetrationBA = findAxisOfLeastPenetration(indexA, worldPolyA, worldPolyB);

        size_t indexB;
        float penetrationAB = findAxisOfLeastPenetration(indexB, worldPolyB, worldPolyA);

        if(penetrationBA >= 0 && penetrationAB < 0) {
            penetration = penetrationBA;

            // Face is on A
            normA = VectorHelper::normal(worldPolyA[indexA] - worldPolyA[(indexA + 1) % worldPolyA.size()]);
            normB = -normA;
            return true;
        } else if(penetrationBA < 0 && penetrationAB >= 0) {
            penetration = penetrationAB;

            // Face is on B
            normB = VectorHelper::normal(worldPolyB[indexB] - worldPolyB[(indexB + 1) % worldPolyB.size()]);
            normA = -normB;
            return true;
        } else if(penetrationBA >= 0 && penetrationAB >= 0) {
            if(penetrationBA > penetrationAB) {
                penetration = penetrationBA;

                // Face is on A
                normA = VectorHelper::normal(worldPolyA[indexA] - worldPolyA[(indexA + 1) % worldPolyA.size()]);
                normB = -normA;
            } else {
                penetration = penetrationAB;
                
                // Face is on B
                normB = VectorHelper::normal(worldPolyB[indexB] - worldPolyB[(indexB + 1) % worldPolyB.size()]);
                normA = -normB;
            }
            return true;
        }

        return false;
    }

    glm::vec2 Collision2DDetectionSystem::computeSupport(const std::vector<glm::vec2>& verts, const glm::vec2& dir) {
        float bestProjection = -std::numeric_limits<float>::max();
        glm::vec2 bestVertex;

        for(size_t i = 0; i < verts.size(); i++) {
            const glm::vec2& v = verts[i];
            float projection = glm::dot(v, dir);

            if(projection > bestProjection) {
                bestProjection = projection;
                bestVertex = v;
            }
        }

        return bestVertex;
    }
    float Collision2DDetectionSystem::findAxisOfLeastPenetration(size_t& faceIndex, const std::vector<glm::vec2>& vertsA, const std::vector<glm::vec2>& vertsB) {
        float bestDistance = -std::numeric_limits<float>::max();
        size_t bestIndex;

        for(size_t i = 0; i < vertsA.size(); i++) {
            const glm::vec2& n = VectorHelper::normal(vertsA[i] - vertsA[(i + 1) % vertsA.size()]);

            // Support point from B along -n
            glm::vec2 sB = computeSupport(vertsB, -n);

            glm::vec2 vA = vertsA[i];

            // Compute penetration distance
            float d = glm::dot(n, sB - vA);

            // Find greatest distance
            if(d > bestDistance) {
                bestDistance = d;
                bestIndex = i;
            }
        }

        faceIndex = bestIndex;
        return bestDistance;
    }
}

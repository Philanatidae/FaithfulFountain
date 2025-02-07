/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMPONENTS_TRANSFORM_COMPONENT_HPP
#define _FAITHFUL_FOUNTAIN_COMPONENTS_TRANSFORM_COMPONENT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <ff/actors/Component.hpp>

#include <tinyformat/tinyformat.h>

namespace ff {
    struct TransformComponent final : public Component<TransformComponent> {
        friend class TransformResetSystem;

        TransformComponent(const glm::vec3& position = glm::vec3(0, 0, 0),
            const glm::quat& rotation = glm::identity<glm::quat>(),
            const float& scale = 1,
            const glm::vec3& stretch = glm::vec3(1, 1, 1));
        TransformComponent(const TransformComponent& transformComp);

        void setPosition(const glm::vec3& position, const bool& reset = false);
        void setPosition(const float& x, const float& y, const bool& reset = false);
        void setPosition(const float& x, const float& y, const float& z, const bool& reset = false);

        void setPositionX(const float& x, const bool& reset = false);
        void setPositionY(const float& y, const bool& reset = false);
        void setPositionZ(const float& z, const bool& reset = false);

        glm::vec3 getPosition() const;
        glm::vec2 get2DPosition() const;
        glm::vec3 getLastPosition() const;
        glm::vec2 getLast2DPosition() const;

        void move(const glm::vec2& dp);
        void move(const glm::vec3& dp);
        void move(const float& x, const float& y, const float& z = 0);

        void set2DRotation(const float& rotation, const bool& reset = false);
        void rotate2D(const float& angle);
        float get2DRotation() const;

        void setRotation(const glm::vec3& eulerAngles, const bool& reset = false);
        void setRotation(const float& pitch, const float& yaw, const float& roll, const bool& reset = false);
        void setRotation(const glm::quat& quat, const bool& reset = false);
        void rotate(const float& angle, const glm::vec3& axis);
        void rotate(const float& angle, const float& x, const float& y, const float& z);
        glm::quat getRotation() const;
        glm::vec3 getEulerRotation() const;

        void setScale(const float& scale, const bool& reset = false);
        float getScale() const;

        void setStretch(const glm::vec2& stretch, const bool& reset = false);
        void setStretch(const glm::vec3& stretch, const bool& reset = false);
        void setStretch(const float& x, const float& y, const bool& reset = false);
        void setStretch(const float& x, const float& y, const float& z, const bool& reset = false);
        void setStretchX(const float& x, const bool& reset = false);
        void setStretchY(const float& y, const bool& reset = false);
        void setStretchZ(const float& z, const bool& reset = false);
        glm::vec3 getStretch() const;
        glm::vec2 get2DStretch() const;

        glm::mat4 getMatrix() const;
        glm::mat4 getLastMatrix() const;

        void getSmoothedProperties(glm::vec3& pos, glm::quat& rotation, float& scale, glm::vec3& stretch, const float& alpha);
        glm::mat4 getSmoothedMatrix(const float& alpha);

        void mirror(const TransformComponent& other,
            const bool& position = true,
            const bool& rotation = true,
            const bool& scale = true,
            const bool& stretch = true);

    private:
        glm::vec3 _position;
        glm::vec3 _lastPosition;
        glm::quat _rotation;
        glm::quat _lastRotation;
        float _scale;
        float _lastScale;
        glm::vec3 _stretch;
        glm::vec3 _lastStretch;

        void resetAll();
    };

    inline std::ostream& operator<<(std::ostream& out, const TransformComponent& rhs) {
        out << tinyformat::format("Position: <%s, %s, %s>, Rotation: <%s, %s, %s>, Scale: %s, Stretch: <%s, %s, %s>",
            rhs.getPosition().x, rhs.getPosition().y, rhs.getPosition().z,
            rhs.getEulerRotation().x, rhs.getEulerRotation().y, rhs.getEulerRotation().z,
            rhs.getScale(),
            rhs.getStretch().x, rhs.getStretch().y, rhs.getStretch().z);
        return out;
    }
}

#endif

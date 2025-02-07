/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_COLOR_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_COLOR_HPP

#include <iostream>
#include <glm/glm.hpp>
#include <ff/io/Serializer.hpp>

namespace ff {
    constexpr float COLOR_EPSILON = 0.001f;

    struct Color {
        Color();
        Color(const float& r, const float& g, const float& b, const float& a = 1.0f);
        Color(uint32_t const& rgba);

        static Color fromHSV(const float& h, const float& s, const float& v, const float& a = 1.0f);

        static Color identity();
        static Color white();
        static Color black();
        static Color transparentBlack();
        static Color transparentWhite();
        static Color red();
        static Color green();
        static Color blue();
        static Color yellow();
        static Color cyan();
        static Color magenta();

        float r;
        float g;
        float b;
        float a;

        float* data();
        float const* data() const;

        Color computePreMultipliedAlpha() const;
        uint32_t toUInt32() const;

        void drawImGuiEdit();

        Color clamp() const;

        inline bool operator==(const Color& other) {
            if(glm::abs(r - other.r) > COLOR_EPSILON) {
                return false;
            }
            if(glm::abs(g - other.g) > COLOR_EPSILON) {
                return false;
            }
            if(glm::abs(b - other.b) > COLOR_EPSILON) {
                return false;
            }
            if(glm::abs(a - other.a) > COLOR_EPSILON) {
                return false;
            }
            return true;
        }

        inline Color& operator*=(const float& rhs) {
            r *= rhs;
            g *= rhs;
            b *= rhs;
            a *= rhs;
            
            return *this;
        }
        inline Color& operator/=(const float& rhs) {
            r /= rhs;
            g /= rhs;
            b /= rhs;
            a /= rhs;
            
            return *this;
        }
        inline Color& operator*=(const Color& rhs) {
            r *= rhs.r;
            g *= rhs.g;
            b *= rhs.b;
            a *= rhs.a;
            
            return *this;
        }
        inline Color& operator/=(const Color& rhs) {
            r /= rhs.r;
            g /= rhs.g;
            b /= rhs.b;
            a /= rhs.a;
            
            return *this;
        }
    };

    template<>
    struct CustomSerializer<Color> {
        static void serialize(Serializer* serializer,
            char const* const label,
            Color& x);
    };

    inline Color operator*(const Color& lhs, const float& rhs) {
        return Color(lhs.r * rhs, lhs.g * rhs, lhs.b * rhs, lhs.a * rhs);
    }
    inline Color operator*(const float& lhs, const Color& rhs) {
        return Color(rhs.r * lhs, rhs.g * lhs, rhs.a * lhs, rhs.a * lhs);
    }
    inline Color operator/(const Color& lhs, const float& rhs) {
        return Color(lhs.r / rhs, lhs.g / rhs, lhs.b / rhs, lhs.a / rhs);
    }
    inline Color operator+(const Color& lhs, const Color& rhs) {
        return Color(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b, lhs.a + rhs.a);
    }
    inline Color operator-(const Color& lhs, const Color& rhs) {
        return Color(lhs.r - rhs.r, lhs.g - rhs.g, lhs.b - rhs.b, lhs.a - rhs.a);
    }
    inline Color operator*(const Color& lhs, const Color& rhs) {
        return Color(lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b, lhs.a * rhs.a);
    }
    inline Color operator/(const Color& lhs, const Color& rhs) {
        return Color(lhs.r / rhs.r, lhs.g / rhs.g, lhs.b / rhs.b, lhs.a / rhs.a);
    }

    inline std::ostream& operator<<(std::ostream& out, const Color& c) {
        out << c.r << " " << c.g << " " << c.b << " " << c.a;
        return out;
    }
    inline std::istream& operator>>(std::istream& in, Color& c) {
        in >> c.r;
        in >> c.g;
        in >> c.b;
        in >> c.a;
        return in;
    }
}

#endif

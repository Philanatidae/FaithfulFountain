/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_CLEAR_COLOR_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_CLEAR_COLOR_HPP

#include <iostream>
#include <glm/glm.hpp>

namespace ff {
    constexpr double CLEAR_COLOR_EPSILON = 0.001;

    struct ClearColor {
        ClearColor(const double& r = 1.0f, const double& g = 1.0f, const double& b = 1.0f, const double& a = 1.0f);

        double r;
        double g;
        double b;
        double a;

        static ClearColor identity();
        static ClearColor white();
        static ClearColor black();
        static ClearColor transparentBlack();
        static ClearColor transparentWhite();
        static ClearColor red();
        static ClearColor green();
        static ClearColor blue();
        static ClearColor yellow();
        static ClearColor cyan();
        static ClearColor magenta();

        double* data();
        double const* data() const;

        inline bool operator==(const ClearColor& other) {
            if (glm::abs(r - other.r) > CLEAR_COLOR_EPSILON) {
                return false;
            }
            if (glm::abs(g - other.g) > CLEAR_COLOR_EPSILON) {
                return false;
            }
            if (glm::abs(b - other.b) > CLEAR_COLOR_EPSILON) {
                return false;
            }
            if (glm::abs(a - other.a) > CLEAR_COLOR_EPSILON) {
                return false;
            }
            return true;
        }

        inline ClearColor& operator*=(const double& rhs) {
            r *= rhs;
            g *= rhs;
            b *= rhs;
            a *= rhs;

            return *this;
        }
        inline ClearColor& operator/=(const double& rhs) {
            r /= rhs;
            g /= rhs;
            b /= rhs;
            a /= rhs;

            return *this;
        }
        inline ClearColor& operator*=(const ClearColor& rhs) {
            r *= rhs.r;
            g *= rhs.g;
            b *= rhs.b;
            a *= rhs.a;

            return *this;
        }
        inline ClearColor& operator/=(const ClearColor& rhs) {
            r /= rhs.r;
            g /= rhs.g;
            b /= rhs.b;
            a /= rhs.a;

            return *this;
        }
    };

    inline ClearColor operator*(const ClearColor& lhs, const float& rhs) {
        return ClearColor(lhs.r * rhs, lhs.g * rhs, lhs.b * rhs, lhs.a * rhs);
    }
    inline ClearColor operator*(const float& lhs, const ClearColor& rhs) {
        return ClearColor(rhs.r * lhs, rhs.g * lhs, rhs.a * lhs, rhs.a * lhs);
    }
    inline ClearColor operator/(const ClearColor& lhs, const float& rhs) {
        return ClearColor(lhs.r / rhs, lhs.g / rhs, lhs.b / rhs, lhs.a / rhs);
    }
    inline ClearColor operator+(const ClearColor& lhs, const ClearColor& rhs) {
        return ClearColor(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b, lhs.a + rhs.a);
    }
    inline ClearColor operator-(const ClearColor& lhs, const ClearColor& rhs) {
        return ClearColor(lhs.r - rhs.r, lhs.g - rhs.g, lhs.b - rhs.b, lhs.a - rhs.a);
    }
    inline ClearColor operator*(const ClearColor& lhs, const ClearColor& rhs) {
        return ClearColor(lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b, lhs.a * rhs.a);
    }
    inline ClearColor operator/(const ClearColor& lhs, const ClearColor& rhs) {
        return ClearColor(lhs.r / rhs.r, lhs.g / rhs.g, lhs.b / rhs.b, lhs.a / rhs.a);
    }

    inline std::ostream& operator<<(std::ostream& out, const ClearColor& c) {
        out << c.r << " " << c.g << " " << c.b << " " << c.a;
        return out;
    }
    inline std::istream& operator>>(std::istream& in, ClearColor& c) {
        in >> c.r;
        in >> c.g;
        in >> c.b;
        in >> c.a;
        return in;
    }
}

#endif

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/Color.hpp>

#include <glm/glm.hpp>

#include <ff/Console.hpp>

#include <imgui.h>

namespace ff {
    Color::Color()
        :Color(Color::white()) {
    }
    Color::Color(const float& r, const float& g, const float& b, const float& a)
        :r(r),g(g),b(b),a(a) {
    }
    Color::Color(uint32_t const& rgba)
        :r(((rgba >> 24) & 0xFF) / 255.0f),
        g(((rgba >> 16) & 0xFF) / 255.0f),
        b(((rgba >> 8) & 0xFF) / 255.0f),
        a((rgba & 0xFF) / 255.0f) {
    }

    Color Color::fromHSV(const float& h, const float& s, const float& v, const float& a) {
        // Source: https://www.codespeedy.com/hsv-to-rgb-in-cpp/
        ff::Color color;

        // Conversion values
        float tempH = h * 360.0f;
        tempH /= 60.0f;
        int i = glm::floor(tempH);

        float C = s * v;
        float X = C * (1.0f - glm::abs(glm::mod(tempH, 2.0f) - 1.0f));
        float m = v - C;

        // There are 6 cases, one for every 60 degrees
        switch(i) {
            case 0:
                color.r = C;
                color.g = X;
                color.b = 0.0f;
                break;
            case 1:
                color.r = X;
                color.g = C;
                color.b = 0.0f;
                break;
            case 2:
                color.r = 0.0f;
                color.g = C;
                color.b = X;
                break;
            case 3:
                color.r = 0.0f;
                color.g = X;
                color.b = C;
                break;
            case 4:
                color.r = X;
                color.g = 0.0f;
                color.b = C;
                break;
            case 5:
                color.r = C;
                color.g = 0.0f;
                color.b = X;
                break;
            default:
                color.r = 0.0f;
                color.g = 0.0f;
                color.b = 0.0f;
                FF_CONSOLE_ERROR("Hue out of range.");
                break;
        }

        // Alpha value stays the same
        color.a = a;

        return color;
    }

    Color Color::identity() {
        return Color(1, 1, 1, 1);
    }
    Color Color::white() {
        return Color(1, 1, 1, 1);
    }
    Color Color::black() {
        return Color(0, 0, 0, 1);
    }
    Color Color::transparentBlack() {
        return Color(0, 0, 0, 0);
    }
    Color Color::transparentWhite() {
        return Color(1, 1, 1, 0); 
    }
    Color Color::red() {
        return Color(1, 0, 0, 1);
    }
    Color Color::green() {
        return Color(0, 1, 0, 1);
    }
    Color Color::blue() {
        return Color(0, 0, 1, 1);
    }
    Color Color::yellow() {
        return Color(1, 1, 0, 1);
    }
    Color Color::cyan() {
        return Color(0, 1, 1, 1);
    }
    Color Color::magenta() {
        return Color(1, 0, 1, 1);
    }

    float* Color::data() {
        return &r;
    }
    float const* Color::data() const {
        return &r;
    }

    Color Color::computePreMultipliedAlpha() const {
        return Color(r * a, g * a, b * a, a);
    }
    uint32_t Color::toUInt32() const {
        uint32_t ir = (uint32_t)(r * 255.0f);
        uint32_t ig = (uint32_t)(g * 255.0f);
        uint32_t ib = (uint32_t)(b * 255.0f);
        uint32_t ia = (uint32_t)(a * 255.0f);

        return (ir << 24) | (ig << 16) | (ib << 8) | ia;
    }

    void Color::drawImGuiEdit() {
        ImGui::ColorEdit4("##editv",
            &r,
            0);
    }

    Color Color::clamp() const {
        return Color(glm::clamp(r, 0.0f, 1.0f),
            glm::clamp(g, 0.0f, 1.0f),
            glm::clamp(b, 0.0f, 1.0f),
            glm::clamp(a, 0.0f, 1.0f));
    }

    void CustomSerializer<Color>::serialize(Serializer* serializer,
        char const* const label,
        Color& x) {
        serializer->beginObject(label);
        serializer->serialize("r", x.r);
        serializer->serialize("g", x.g);
        serializer->serialize("b", x.b);
        serializer->serialize("a", x.a);
        serializer->endObject();
    }
}

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMPONENTS_FIELD_FONT_COMPONENT_HPP
#define _FAITHFUL_FOUNTAIN_COMPONENTS_FIELD_FONT_COMPONENT_HPP

#include <ff/actors/Component.hpp>
#include <ff/graphics/FieldFont.hpp>
#include <ff/graphics/Color.hpp>
#include <string>

namespace ff {
    struct FieldFontComponent final : public Component<FieldFontComponent> {
        FieldFontComponent(const std::shared_ptr<FieldFont>& font = nullptr,
            const std::string& content = "",
            const Color& tint = Color(1, 1, 1, 1),
            const bool& hidden = false)
            :font(font),
            content(content),
            tint(tint),
            hidden(hidden) {
        }

        std::shared_ptr<FieldFont> font;
        std::string content;
        Color tint;
        bool hidden;
    };
    
}

#endif

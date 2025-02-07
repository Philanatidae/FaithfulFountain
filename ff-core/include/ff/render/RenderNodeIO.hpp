/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_RENDER_RENDER_NODE_IO_HPP
#define _FF_RENDER_RENDER_NODE_IO_HPP

#include <glm/glm.hpp>
#include <ff/graphics/Color.hpp>

namespace ff {

enum class RenderNodeIOType {
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    MATRIX,
    COLOR,
    BOOL,
    COLOR_TEXTURE_SAMPLE,
    COLOR_TEXTURE_BLIT
};
char const* convertRenderNodeIOTypeToFriendlyString(RenderNodeIOType const& type);

struct IRenderNodeIO {
    IRenderNodeIO() {
    }
    virtual ~IRenderNodeIO() = default;

    virtual char const* getName() const = 0;
    virtual RenderNodeIOType getType() const = 0;
};

template<RenderNodeIOType IOT, typename UT>
struct RenderNodeIO : public IRenderNodeIO {
    RenderNodeIO(char const* const& name,
        UT const& val):_name(name),_val(val) {}
    virtual ~RenderNodeIO() = default;

    char const* getName() const override {
        return _name;
    }
    RenderNodeIOType getType() const override {
        return IOT;
    }

    UT getValue() const {
        return _val;
    }
    void setValue(UT const& val) {
        _val = val;
    }

private:
    char const* const _name;
    UT _val;
};

using IntRenderNodeIO = RenderNodeIO<RenderNodeIOType::INT, int>;
using FloatRenderNodeIO = RenderNodeIO<RenderNodeIOType::FLOAT, float>;
using Vec2RenderNodeIO = RenderNodeIO<RenderNodeIOType::VEC2, glm::vec2>;
using Vec3RenderNodeIO = RenderNodeIO<RenderNodeIOType::VEC3, glm::vec3>;
using Vec4RenderNodeIO = RenderNodeIO<RenderNodeIOType::VEC4, glm::vec4>;
using MatrixRenderNodeIO = RenderNodeIO<RenderNodeIOType::MATRIX, glm::mat4x4>;
using ColorRenderNodeIO = RenderNodeIO<RenderNodeIOType::COLOR, Color>;
using BoolRenderNodeIO = RenderNodeIO<RenderNodeIOType::BOOL, bool>;
using ColorTextureSampleRenderNodeIO = RenderNodeIO<RenderNodeIOType::COLOR_TEXTURE_SAMPLE, std::string>;
using ColorTextureBlitRenderNodeIO = RenderNodeIO<RenderNodeIOType::COLOR_TEXTURE_BLIT, std::string>;

}

#endif


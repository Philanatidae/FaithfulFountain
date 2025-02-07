/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_RENDER_COLOR_TEXTURE_BLIT_RENDER_NODE_HPP
#define _FF_RENDER_COLOR_TEXTURE_BLIT_RENDER_NODE_HPP

#include <ff/render/RenderNode.hpp>
#include <ff/render/RenderNodeIO.hpp>

namespace ff {

struct ColorTextureBlitRenderNode final : public RenderNode<0, 1> {
    static constexpr int OUTPUT_COLOR_TEXTURE = 0;

    ColorTextureBlitRenderNode(std::string const& texture);
    ~ColorTextureBlitRenderNode() = default;

    void setTexture(std::string const& texture);
    std::string getTexture() const;

    void prepare() override;
    void process(float const& alpha) override;

protected:
    void drawImGuiNodeContents() override;

private:
    std::string _texture;
};

}

#endif


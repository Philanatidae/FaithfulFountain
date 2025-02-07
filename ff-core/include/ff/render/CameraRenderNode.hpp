/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_RENDER_CAMERA_RENDER_NODE_HPP
#define _FF_RENDER_CAMERA_RENDER_NODE_HPP

#include <ff/render/RenderNode.hpp>
#include <ff/render/RenderNodeIO.hpp>

namespace ff {

struct CameraRenderNode final : public RenderNode<0, 3> {
    static constexpr int OUTPUT_COMBINED_MAT = 0;
    static constexpr int OUTPUT_TRANSFORM_MAT = 1;
    static constexpr int OUTPUT_PERSPECTIVE_MAT = 2;

    CameraRenderNode(std::string const& camera);
    ~CameraRenderNode() = default;

    void setCamera(std::string const& camera);
    std::string getCamera() const;

    void prepare() override;
    void process(float const& alpha) override;

protected:
    void drawImGuiNodeContents() override;

private:
    std::string _camera;
};

}

#endif


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_IDEPTH_STENCIL_STATE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_IDEPTH_STENCIL_STATE_HPP

namespace ff {
    enum class DepthCompareFunction {
        NEVER,
        ALWAYS,
        LESS,
        EQUAL,
        LESS_EQUAL,
        GREATER,
        GREATER_EQUAL,
        NOT_EQUAL
    };

    // @todo Support stencils

    class IDepthStencilState {
    public:
        IDepthStencilState(DepthCompareFunction const& depthCompareFunction,
            bool const& depthWriteEnabled);
        virtual ~IDepthStencilState() = default;

        DepthCompareFunction getDepthCompareFunction() const;

        bool getDepthWriteEnabled() const;

    private:
        DepthCompareFunction _depthCompareFunction;
        bool _depthWriteEnabled;
    };

    class NullDepthStencilState : public IDepthStencilState {
    public:
        NullDepthStencilState(const DepthCompareFunction& depthCompareFunction,
                              const bool& depthWriteEnabled)
            :IDepthStencilState(depthCompareFunction, depthWriteEnabled) {
        }
        ~NullDepthStencilState() = default;
    };
}

#endif

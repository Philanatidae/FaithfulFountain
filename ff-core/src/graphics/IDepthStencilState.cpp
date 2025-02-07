/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/IDepthStencilState.hpp>

namespace ff {
    IDepthStencilState::IDepthStencilState(DepthCompareFunction const& depthCompareFunction,
        bool const& depthWriteEnabled)
        :_depthCompareFunction(depthCompareFunction),
        _depthWriteEnabled(depthWriteEnabled) {
    }

    DepthCompareFunction IDepthStencilState::getDepthCompareFunction() const {
        return _depthCompareFunction;
    }

    bool IDepthStencilState::getDepthWriteEnabled() const {
        return _depthWriteEnabled;
    }
}

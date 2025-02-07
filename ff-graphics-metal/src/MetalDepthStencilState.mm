/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalDepthStencilState.hmm>

namespace ff {
    MTLCompareFunction convertDepthCompareFunctionToMetalDepthCompareFunction(DepthCompareFunction depthCompareFunction) {
        switch(depthCompareFunction) {
            case DepthCompareFunction::NEVER:
                return MTLCompareFunctionNever;
            case DepthCompareFunction::LESS:
                return MTLCompareFunctionLess;
            case DepthCompareFunction::EQUAL:
                return MTLCompareFunctionEqual;
            case DepthCompareFunction::LESS_EQUAL:
                return MTLCompareFunctionLessEqual;
            case DepthCompareFunction::GREATER:
                return MTLCompareFunctionGreater;
            case DepthCompareFunction::GREATER_EQUAL:
                return MTLCompareFunctionGreaterEqual;
            default:
            FF_CONSOLE_ERROR("Unknown conversion of depth compare function to Metal depth compare function.");
            case DepthCompareFunction::ALWAYS:
                return MTLCompareFunctionNever;
        }
    }

    MetalDepthStencilState::MetalDepthStencilState(MetalGraphicsDevice& graphicsDevice,
        const DepthCompareFunction& depthCompareFunction,
        const bool& depthWriteEnabled)
        :IDepthStencilState(depthCompareFunction, depthWriteEnabled),
        _depthCompareFunction(depthCompareFunction),
        _depthWriteEnabled(depthWriteEnabled) {
        MTLDepthStencilDescriptor* depthStencilDescriptor = [MTLDepthStencilDescriptor new];
        depthStencilDescriptor.depthCompareFunction = convertDepthCompareFunctionToMetalDepthCompareFunction(depthCompareFunction);
        depthStencilDescriptor.depthWriteEnabled = depthWriteEnabled;

        // @todo Stencils

        _depthStencilState = [graphicsDevice.getDevice() newDepthStencilStateWithDescriptor:depthStencilDescriptor];
    }
    MetalDepthStencilState::~MetalDepthStencilState() {
    }

    id<MTLDepthStencilState> MetalDepthStencilState::getDepthStencilState() const {
        return _depthStencilState;
    }
}

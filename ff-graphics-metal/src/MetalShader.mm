/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalShader.hmm>

namespace ff {
    MetalShader::MetalShader(MetalGraphicsDevice& graphicsDevice,
        id<MTLFunction> vertexFunction,
        id<MTLFunction> fragmentFunction,
        const std::unordered_set<int>& vertexImageSlots,
        const std::unordered_set<int>& fragmentImageSlots,
        const std::unordered_set<int>& vertexUBSlots,
        const std::unordered_set<int>& fragmentUBSlots)
        :_vertexFunction(vertexFunction),
        _fragmentFunction(fragmentFunction),
        _vertexImageSlots(vertexImageSlots),
        _fragmentImageSlots(fragmentImageSlots),
        _vertexUBSlots(vertexUBSlots),
        _fragmentUBSlots(fragmentUBSlots) {
    }
    MetalShader::~MetalShader() {
    }

    id<MTLFunction> MetalShader::getVertexFunction() const {
        return _vertexFunction;
    }
    id<MTLFunction> MetalShader::getFragmentFunction() const {
        return _fragmentFunction;
    }

    bool MetalShader::isImageSlotInVertexFunction(const int& slot) const {
        return _vertexImageSlots.find(slot) != _vertexImageSlots.end();
    }
    bool MetalShader::isImageSlotInFragmentFunction(const int& slot) const {
        return _fragmentImageSlots.find(slot) != _fragmentImageSlots.end();
    }
    bool MetalShader::isUniformBufferSlotInVertexFunction(const int& slot) const {
        return _vertexUBSlots.find(slot) != _vertexUBSlots.end();
    }
    bool MetalShader::isUniformBufferSlotInFragmentFunction(const int& slot) const {
        return _fragmentUBSlots.find(slot) != _fragmentUBSlots.end();
    }
}

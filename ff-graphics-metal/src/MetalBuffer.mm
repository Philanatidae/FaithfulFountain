/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalBuffer.hmm>
#include <stdint.h>
#include <cstring>

namespace ff {
    MetalBuffer::MetalBuffer(MetalGraphicsDevice* const& graphicsDevice,
        size_t size)
        :_graphicsDevice(graphicsDevice),
        _size(size) {
        MTLResourceOptions bufferOptions = 0;
        _buffer = [graphicsDevice->getDevice() newBufferWithLength:(NSUInteger)_size options:bufferOptions];
    }
    MetalBuffer::~MetalBuffer() {
    }

    const size_t& MetalBuffer::getSize() const {
        return _size;
    }

    void MetalBuffer::bufferData(void const* ptr, const size_t& size, const size_t& start) {
        intptr_t bufferContentsPtr = (intptr_t)[_buffer contents];
        bufferContentsPtr += start;
        std::memcpy((void*)bufferContentsPtr, ptr, size);
    }

    id<MTLBuffer> MetalBuffer::getBuffer() {
        return _buffer;
    }
}

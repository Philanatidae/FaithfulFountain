/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/VertexBuffer.hpp>

#include <ff/Console.hpp>

namespace ff {
    IVertexBuffer::IVertexBuffer(VertexBufferImp* imp, int const& elementSize, int const& count)
        : _imp(imp),_elementSize(elementSize),_count(count) {
    }

    VertexBufferImp* IVertexBuffer::getImp() const {
        return _imp;
    }

    void IVertexBuffer::bufferData(void const* const& ptr, int const& count, int const& start) {
        // Double check that we are in range
        if (start + count > _count) {
            FF_CONSOLE_ERROR("Attempting to buffer data outside of vertex buffer range.");
            return;
        }

        _imp->bufferData(ptr, _elementSize * count, _elementSize * start);
    }
    int IVertexBuffer::getCount() {
        return _count;
    }
}

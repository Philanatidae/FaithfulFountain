/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/IndexBuffer.hpp>

#include <ff/Console.hpp>

namespace ff {
    IIndexBuffer::IIndexBuffer(IndexBufferImp* imp,
        int const& elementSize,
        int const& count,
        IndexType const& indexType)
        :_imp(imp),
        _elementSize(elementSize),
        _count(count),
        _indexType(indexType) {
    }

    IndexBufferImp* IIndexBuffer::getImp() const {
        return _imp;
    }

    void IIndexBuffer::bufferData(void const* const& ptr, const int& count, const int& start) {
        // Double check that we are in range
        if (start + count > _count) {
            FF_CONSOLE_ERROR("Attempting to buffer data outside of index buffer range.");
            return;
        }

        _imp->bufferData(ptr, _elementSize * count, _elementSize * start);
    }
    int IIndexBuffer::getElementSize() const {
        return _elementSize;
    }
    int IIndexBuffer::getCount() const {
        return _count;
    }
    IndexType IIndexBuffer::getIndexType() const {
        return _indexType;
    }
}

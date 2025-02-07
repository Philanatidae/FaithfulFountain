/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_INDEX_BUFFER_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_INDEX_BUFFER_HPP

#include <ff/graphics/IndexType.hpp>

namespace ff {
    class IndexBufferImp {
    public:
        virtual ~IndexBufferImp() = default;

        virtual void bufferData(void const* const& data, const int& size, const int& start) = 0;
    };
    
    class NullIndexBufferImp : public IndexBufferImp {
    public:
        NullIndexBufferImp() { }
        virtual ~NullIndexBufferImp() = default;

        void bufferData(void const* const& ptr, const int& size, const int& start) override {
        }
    };

    class IIndexBuffer {
    public:
        IIndexBuffer(IndexBufferImp* imp, int const& elementSize, int const& count, IndexType const& indexType);
        virtual ~IIndexBuffer() = default;

        IndexBufferImp* getImp() const;

        void bufferData(void const* const& ptr, const int& count, const int& start = 0);
        int getElementSize() const;
        int getCount() const;
        IndexType getIndexType() const;

    private:
        IndexBufferImp* _imp;

        int _elementSize;
        int _count;
        IndexType _indexType;
    };

    template<IndexType T = IndexType::UINT32>
    class IndexBuffer : public IIndexBuffer {
    public:
        using type = typename IndexTypeUnderlyingType<T>::type;

        IndexBuffer(IndexBufferImp* imp, int const& count);
        virtual ~IndexBuffer() = default;

        void bufferData(typename IndexBuffer<T>::type const* const& data, const int& count, const int& start = 0);
    };

    template<IndexType T>
    IndexBuffer<T>::IndexBuffer(IndexBufferImp* imp, int const& count)
        : IIndexBuffer(imp, sizeof(IndexBuffer<T>::type), count, T) {
    }

    template<IndexType T>
    void IndexBuffer<T>::bufferData(typename IndexBuffer<T>::type const* const& data, const int& count, const int& start) {
        IIndexBuffer::bufferData(data, count, start);
    }
}

#endif

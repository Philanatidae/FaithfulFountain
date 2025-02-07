/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_VERTEX_BUFFER_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_VERTEX_BUFFER_HPP

namespace ff {
    class VertexBufferImp {
    public:
        virtual ~VertexBufferImp() = default;

        virtual void bufferData(void const* const& ptr, int const& size, int const& start) = 0;
    };
    
    class NullVertexBufferImp : public VertexBufferImp {
    public:
        NullVertexBufferImp() { }
        virtual ~NullVertexBufferImp() = default;

        void bufferData(void const* const& ptr, int const& size, int const& start) override {
        }
    };

    class IVertexBuffer {
    public:
        IVertexBuffer(VertexBufferImp* imp, int const& elementSize, int const& count);
        virtual ~IVertexBuffer() = default;

        VertexBufferImp* getImp() const;

        void bufferData(void const* const& ptr, int const& count, int const& start = 0);
        int getCount();

    private:
        VertexBufferImp* _imp;

        int _elementSize;
        int _count;
    };

    template<typename T>
    class VertexBuffer : public IVertexBuffer {
    public:
        VertexBuffer(VertexBufferImp* imp, int const& count);
        virtual ~VertexBuffer() = default;

        void bufferData(T const* const& ptr, int const& count, int const& start = 0);
    };

    template<typename T>
    VertexBuffer<T>::VertexBuffer(VertexBufferImp* imp, int const& count)
        :IVertexBuffer(imp, sizeof(T), count) {
    }

    template<typename T>
    void VertexBuffer<T>::bufferData(T const* const& ptr, int const& count, int const& start) {
        IVertexBuffer::bufferData(ptr, count, start);
    }
}

#endif

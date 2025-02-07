/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_UNIFORM_BUFFER_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_UNIFORM_BUFFER_HPP

namespace ff {
    class UniformBufferImp {
    public:
        virtual ~UniformBufferImp() = default;

        virtual void bufferData(void const* const& ptr, int const& size, int const& start) = 0;
    };

    class NullUniformBufferImp : public UniformBufferImp {
    public:
        NullUniformBufferImp() { }
        virtual ~NullUniformBufferImp() = default;

    protected:
        void bufferData(void const* const& ptr, int const& size, int const& start) override {
        }
    };

    class IUniformBuffer {
    public:
        IUniformBuffer(UniformBufferImp* imp, int const& size);
        virtual ~IUniformBuffer() = default;

        UniformBufferImp* getImp() const;

        void bufferData(void const* ptr);

    private:
        UniformBufferImp* _imp;
        int _size;
    };

    template<typename T>
    class UniformBuffer : public IUniformBuffer {
    public:
        UniformBuffer(UniformBufferImp* imp);
        virtual ~UniformBuffer() = default;

        void bufferData(T const* const& ptr);
    };

    template<typename T>
    UniformBuffer<T>::UniformBuffer(UniformBufferImp* imp)
        : IUniformBuffer(imp, sizeof(T)) {
    }

    template<typename T>
    void UniformBuffer<T>::bufferData(T const* const& ptr) {
        IUniformBuffer::bufferData(ptr);
    }
}

#endif

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/UniformBuffer.hpp>

namespace ff {
    IUniformBuffer::IUniformBuffer(UniformBufferImp* imp, int const& size)
        :_imp(imp),
        _size(size) {
    }

    UniformBufferImp* IUniformBuffer::getImp() const {
        return _imp;
    }

    void IUniformBuffer::bufferData(void const* ptr) {
        _imp->bufferData(ptr, _size, 0);
    }
}

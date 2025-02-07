/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLBuffer.hpp>

#include <ff-graphics-opengl/GLGlobalState.hpp>

#include <cstring>

#include <ff/Console.hpp>

namespace ff {
    GLBuffer::GLBuffer(GLGraphicsDevice* const& graphicsDevice, GLenum target, size_t elementSize, size_t count)
        :_graphicsDevice(graphicsDevice),
        _target(target),
        _elementSize(elementSize),
        _totalSize(elementSize * count),
        _hasBoundBuffer(false) {
        FF_GL_CALL(glGenBuffers, 1, &_buffer);
        bind();
    }
    GLBuffer::~GLBuffer() {
        FF_GL_CALL(glDeleteBuffers, 1, &_buffer);
    }

    const size_t& GLBuffer::getElementSize() const {
        return _elementSize;
    }
    const size_t& GLBuffer::getTotalSize() const {
        return _totalSize;
    }
    size_t GLBuffer::getCount() const {
        return _totalSize / _elementSize;
    }

    void GLBuffer::bufferData(void const* ptr, const size_t& count, const size_t& start) {
        // @todo Safeguard buffer overflow?

        // @todo GL_STREAM_DRAW/GL_DYNAMIC_DRAW/GL_STATIC_DRAW need to be handled correctly. This
        // means hinting from FF. We will already have to do this to make memory private on the
        // Metal side, so we can do both at once.

        // According to the documentation, `glBufferData` is more preferable
        // than `glBufferSubData`, so we'll use `glBufferData` when we can.
        // (for copying the entire buffer)
        //
        // After further research, glBufferSubData is slow on some drivers
        // (Galaxy S7 Edge GPU for example). This is because OpenGL is stalling
        // the CPU until the previous operations have concluded (synchronize)
        // because it isn't smart enough (some drivers anyway) to know that
        // we aren't modifying existing buffers after drawing (because on Metal,
        // you have to buffer all your data then encode draw calls, and wait
        // until you commit the buffer before the GPU begins its work, so
        // you have to design your render calls to not overwrite buffers
        // that you are drawing or you will lost data). Since we are targeting
        // OpenGL ES 3.0, we can use glMapBufferRange insted, which we have
        // the ability to get a pointer to the buffer while forcing OpenGL
        // to not synchronize the buffer. Because of limitations of Metal,
        // this is safe, but we might consider adding a hard safeguard in, at
        // least for debugging purposes, warning if a buffer written this frame
        // is potentially being written over.
        bind();
        if (start == 0 && count == getCount()) {
            FF_GL_CALL(glBufferData, _target, getTotalSize(), ptr, GL_STREAM_DRAW);
        }
        else {
            if (!_hasBoundBuffer) {
                // We have to "allocate" data in the buffer before
                // we can call `glBufferSubData`. We can't use the
                // above if because it can go outside the memory
                // of `ptr`. We'll fill the buffer with null data
                // and then call `glBufferSubData` like normal.
                FF_GL_CALL(glBufferData, _target, getTotalSize(), nullptr, GL_STREAM_DRAW);
                _hasBoundBuffer = true;
            }
            // FF_GL_CALL(glBufferSubData(_target, start * getElementSize(), count * getElementSize(), ptr));

            GLbitfield mapBufferRangeBits = GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;

            // Use invalidate range bit for separate memory, don't use for shared memory
            // https://community.arm.com/developer/tools-software/graphics/f/discussions/8750/glmapbufferrange-and-glunmapbuffer-performance-on-the-mali-t880
            if(_graphicsDevice->getGPUMemoryType() == GPUMemoryType::DISCRETE) {
                mapBufferRangeBits |= GL_MAP_INVALIDATE_RANGE_BIT;
            }
            
            // See above for why we are using glMapBufferRange instead of glBufferSubData
            void* bufferContentsPtr = FF_GL_CALL(glMapBufferRange, _target,
                start,
                count,
                mapBufferRangeBits);
            FF_ASSERT(bufferContentsPtr, "Could not map buffer range.");
            std::memcpy((void*)bufferContentsPtr, ptr, count);
            FF_GL_CALL(glUnmapBuffer, _target);
        }
    }

    void GLBuffer::bind() {
        _graphicsDevice->getGlobalState()->bindBuffer(_target, _buffer);
    }

    GLuint GLBuffer::getGLBuffer() const {
        return _buffer;
    }
}

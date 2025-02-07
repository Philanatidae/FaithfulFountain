/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGLS_CONFIG_ATTRIB_LIST_HPP
#define _FAITHFUL_FOUNTAIN_SUPPORT_EGL_EGLS_CONFIG_ATTRIB_LIST_HPP

#include <ff-support-egl/egl.hpp>

#include <memory>
#include <unordered_map>

namespace ff {
    enum class EGLSRenderableTypes : EGLint {
        OPENGL_ES = EGL_OPENGL_ES_BIT
    };
    enum class EGLSSurfaceTypes : EGLint {
        WINDOW = EGL_WINDOW_BIT
    };

    class EGLSConfigAttribListBuilder;

    class EGLSConfigAttribList final {
    public:
        static EGLSConfigAttribListBuilder begin();

        EGLSConfigAttribList(std::unique_ptr<EGLint[]>&& attribs);
        ~EGLSConfigAttribList() = default;

        EGLSConfigAttribList(EGLSConfigAttribList&& other);
        EGLSConfigAttribList& operator=(EGLSConfigAttribList&& other) noexcept;

        EGLint const* getEGLConfigAttribList() const;

    private:
        std::unique_ptr<EGLint[]> _attribs;
    };

    class EGLSConfigAttribListBuilder {
    friend class EGLSConfigAttribList;

    public:
        ~EGLSConfigAttribListBuilder() = default;

        EGLSConfigAttribListBuilder& setRenderableType(EGLSRenderableTypes const& type);

        EGLSConfigAttribListBuilder& setSurfaceType(EGLSSurfaceTypes const& type);

        EGLSConfigAttribListBuilder& setRedSize(EGLint const& size);
        EGLSConfigAttribListBuilder& setGreenSize(EGLint const& size);
        EGLSConfigAttribListBuilder& setBlueSize(EGLint const& size);
        EGLSConfigAttribListBuilder& setAlphaSize(EGLint const& size);

        EGLSConfigAttribList end();

    private:
        std::unordered_map<EGLint, EGLint> _attribMap;

        EGLSConfigAttribListBuilder() = default;

        template<EGLint key>
        inline void setAttrib(EGLint value) {
            if(_attribMap.find(value) == _attribMap.end()) {
                _attribMap.emplace(key, value);
                return;
            }
            _attribMap[key] = value;
        }
    };
}

#endif

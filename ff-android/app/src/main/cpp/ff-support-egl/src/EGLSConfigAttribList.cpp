/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-support-egl/EGLSConfigAttribList.hpp>

namespace ff {
    EGLSConfigAttribListBuilder EGLSConfigAttribList::begin() {
        return EGLSConfigAttribListBuilder();
    }

    EGLSConfigAttribList::EGLSConfigAttribList(std::unique_ptr<EGLint[]>&& attribs)
        :_attribs(std::move(attribs)) {
    }

    EGLSConfigAttribList::EGLSConfigAttribList(EGLSConfigAttribList&& other)
            :_attribs(std::move(other._attribs)) {
        other._attribs = nullptr;
    }
    EGLSConfigAttribList& EGLSConfigAttribList::operator=(EGLSConfigAttribList&& other) noexcept {
        _attribs = std::move(other._attribs);

        other._attribs = nullptr;

        return *this;
    }

    EGLint const* EGLSConfigAttribList::getEGLConfigAttribList() const {
        return _attribs.get();
    }

    EGLSConfigAttribListBuilder& EGLSConfigAttribListBuilder::setRenderableType(EGLSRenderableTypes const& type) {
        setAttrib<EGL_RENDERABLE_TYPE>((EGLint)type);
        return *this;
    }

    EGLSConfigAttribListBuilder& EGLSConfigAttribListBuilder::setSurfaceType(EGLSSurfaceTypes const& type) {
        setAttrib<EGL_SURFACE_TYPE>((EGLint)type);
        return *this;
    }

    EGLSConfigAttribListBuilder& EGLSConfigAttribListBuilder::setRedSize(EGLint const& size) {
        setAttrib<EGL_RED_SIZE>(size);
        return *this;
    }

    EGLSConfigAttribListBuilder& EGLSConfigAttribListBuilder::setGreenSize(EGLint const& size) {
        setAttrib<EGL_GREEN_SIZE>(size);
        return *this;
    }
    EGLSConfigAttribListBuilder& EGLSConfigAttribListBuilder::setBlueSize(EGLint const& size) {
        setAttrib<EGL_BLUE_SIZE>(size);
        return *this;
    }
    EGLSConfigAttribListBuilder& EGLSConfigAttribListBuilder::setAlphaSize(EGLint const& size) {
        setAttrib<EGL_ALPHA_SIZE>(size);
        return *this;
    }


    EGLSConfigAttribList EGLSConfigAttribListBuilder::end() {
        size_t attribsSize = 2 * _attribMap.size() + 1;
        std::unique_ptr<EGLint[]> attribs(new EGLint[attribsSize]);

        int i = 0;
        for(const auto& attribPair : _attribMap) {
            attribs[i] = attribPair.first;
            attribs[i + 1] = attribPair.second;
            i += 2;
        }
        attribs[attribsSize - 1] = EGL_NONE;

        return EGLSConfigAttribList(std::move(attribs));
    }
}

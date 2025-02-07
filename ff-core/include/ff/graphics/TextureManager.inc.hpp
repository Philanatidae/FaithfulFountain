/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_GRAPHICS_TEXTURE_MANAGER_HPP
#include <ff/graphics/TextureManager.hpp>
#endif

#include <ff/Console.hpp>

namespace ff {

template<typename T>
TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::TextureManager() {
}

template<typename T>
void TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::initialize() {
    Locator::getMessageBus().addListener<ResizeEvent>(this);
}

template<typename T>
bool TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::createTexture(std::string const& name,
    typename T::bufferInfoType const& format,
    int const& width,
    int const& height,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage) {
    return addTexture(name,
        Locator::getGraphicsDevice().createTexture<T>(format,
            width,
            height,
            flags,
            usage));
}
template<typename T>
bool TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::addTexture(std::string const& name,
    ResourceHandle<T> const& tex) {
    if(hasTexture(name)) {
        FF_CONSOLE_WARN("Texture `%s` already exists.", name);
        return false;
    }
    _textures.insert(std::make_pair(name, tex));
    return true;
}
template<typename T>
bool TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::createReactiveTexture(std::string const& name,
    typename T::bufferInfoType const& format,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage,
    float const& framebufferScale) {
    return createReactiveTexture(name,
        format,
        flags,
        usage,
        framebufferScale,
        framebufferScale);
}
template<typename T>
bool TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::createReactiveTexture(std::string const& name,
    typename T::bufferInfoType const& format,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage,
    float const& framebufferHScale,
    float const& framebufferVScale) {
    FF_ASSERT(framebufferHScale > 0 && framebufferVScale > 0,
        "Framebuffer scales must be positive (%s, %s).", framebufferHScale, framebufferVScale);

    if(!addTexture(name,
        Locator::getGraphicsDevice().createTextureFromBackBufferDimensions<T>(format,
            flags,
            usage,
            framebufferHScale,
            framebufferVScale,
            name))) {
        return false;
    }

    _reactiveTextures.insert(name);

    return true;
}

template<typename T>
std::vector<std::string> TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::getTextures() const {
    std::vector<std::string> names;
    for(auto const& ts : _textures) {
        names.push_back(ts.first);
    }
    return names;
}

template<typename T>
bool TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::removeTexture(std::string const& name) {
    if(!hasTexture(name)) {
        return false;
    }
    _textures.erase(name);
    if(_reactiveTextures.find(name) != _reactiveTextures.end()) {
        _reactiveTextures.erase(name);
    }
    return true;
}
template<typename T>
bool TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::hasTexture(std::string const& name) const {
    return _textures.find(name) != _textures.end();
}

template<typename T>
ResourceHandle<T> const TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::getTexture(std::string const& name) const {
    if(!hasTexture(name)) {
        return ResourceHandle<T>::createNullResource();
    }
    return _textures.at(name);
}
template<typename T>
ResourceHandle<T> TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::getTexture(std::string const& name) {
    if(!hasTexture(name)) {
        return ResourceHandle<T>::createNullResource();
    }
    return _textures.at(name);
}
template<typename T>
ImTextureID TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::getTextureAsImTextureID(std::string const& name) const {
    if(!hasTexture(name)) {
        return nullptr;
    }
    return reinterpret_cast<ImTextureID>(&_textures.at(name));
}
template<typename T>
ImTextureID TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::getTextureAsImTextureID(std::string const& name) {
    if(!hasTexture(name)) {
        return nullptr;
    }
    return reinterpret_cast<ImTextureID>(&_textures.at(name));
}

template<typename T>
bool TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>::processEvent(ResizeEvent const& evt) {
    for(auto const& reactiveTextures : _reactiveTextures) {
        _textures[reactiveTextures].reload();
    }

    return false;
}

}

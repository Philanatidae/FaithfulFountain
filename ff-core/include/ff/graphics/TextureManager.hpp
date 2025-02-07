/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_GRAPHICS_TEXTURE_MANAGER_HPP
#define _FF_GRAPHICS_TEXTURE_MANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <type_traits>

#include <glm/glm.hpp>

#include <imgui.h>

#include <ff/graphics/Texture.hpp>
#include <ff/resources/ResourceHandle.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/events/ResizeEvent.hpp>

#include <ff/graphics/TextureManagerFwd.hpp>

namespace ff {

template<typename T>
class TextureManager<T, std::enable_if_t<std::is_base_of<ITexture, T>::value>>  final : public EventListener<ResizeEvent> {
public:
    TextureManager();
    ~TextureManager() = default;

    void initialize();

    bool createTexture(std::string const& name,
        typename T::bufferInfoType const& format,
        int const& width,
        int const& height,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage);
    bool addTexture(std::string const& name,
        ResourceHandle<T> const& tex);
    bool createReactiveTexture(std::string const& name,
        typename T::bufferInfoType const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        float const& framebufferScale);
    bool createReactiveTexture(std::string const& name,
        typename T::bufferInfoType const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        float const& framebufferHScale,
        float const& framebufferVScale);

    std::vector<std::string> getTextures() const;

    bool removeTexture(std::string const& name);
    bool hasTexture(std::string const& name) const;

    ResourceHandle<T> const getTexture(std::string const& name) const;
    ResourceHandle<T> getTexture(std::string const& name);
    ImTextureID getTextureAsImTextureID(std::string const& name) const;
    ImTextureID getTextureAsImTextureID(std::string const& name);

    bool processEvent(ResizeEvent const& evt) override;

private:
    std::unordered_map<std::string, ResourceHandle<T>> _textures;
    std::unordered_set<std::string> _reactiveTextures;
};

}

#include <ff/Locator.hpp>
#include <ff/graphics/TextureManager.inc.hpp>


#endif


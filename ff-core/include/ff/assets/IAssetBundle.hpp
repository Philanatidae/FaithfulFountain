/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSETS_IASSET_BUNDLE_HPP
#define _FAITHFUL_FOUNTAIN_ASSETS_IASSET_BUNDLE_HPP

#include <ff/resources/ResourceHandle.hpp>
#include <ff/io/BinaryReader.hpp>

#include <unordered_map>
#include <map>
#include <typeindex>
#include <string>
#include <memory>
#include <nlohmann/json.hpp>

#define FF_ASSET_TYPE_CHECK(obj, typeName) FF_ASSERT(obj["type"] == typeName, "Incorrect type. Expected `%s`, type is `%s`.", typeName, obj["type"]);

namespace ff {
    class IAssetBundle;

    template<typename T>
    struct DefaultAssetLoader {
        inline static T* load(IAssetBundle& assetBundle, const nlohmann::json& assetObject) {
            return new T(assetBundle, assetObject);
        }
    };

    class IAssetBundle {
    public:
        IAssetBundle();
        virtual ~IAssetBundle();

        void init();

        void releaseCachedContent();

        template<typename T, typename L = DefaultAssetLoader<T>>
        ResourceHandle<T> load(const std::string& name, const bool& cacheInternally = true);

        nlohmann::json getBundleIndexObject() const;
        bool isValidAsset(const std::string& name) const;
        nlohmann::json getAssetObject(const std::string& name) const;
        virtual std::shared_ptr<BinaryReader> getAssetReader(const std::string& path) = 0;

    private:
        nlohmann::json _indexObject;

        std::map<std::pair<std::string, std::type_index>, IResourceHandle*> _cache;

        void detectAssets();

    protected:
        virtual void onInit() = 0;
        virtual nlohmann::json loadIndexObject() = 0;
    };

    class NullAssetBundle : public IAssetBundle {
    public:
        NullAssetBundle();
        virtual ~NullAssetBundle();

        virtual std::shared_ptr<BinaryReader> getAssetReader(const std::string& path);

    protected:
        virtual void onInit();
        virtual nlohmann::json loadIndexObject();
    };
}

#include <ff/Console.hpp>
#include <typeinfo>

namespace ff {
    template<typename T, typename L>
    ResourceHandle<T> IAssetBundle::load(const std::string& name, const bool& cacheInternally) {
        std::pair<std::string, std::type_index> key = std::make_pair(name, std::type_index(typeid(T)));
        auto cacheIt = _cache.find(key);
        if(cacheIt == _cache.end()) {
            FF_ASSERT(isValidAsset(name), "Invalid asset name (%s).", name);

            ResourceHandle<T> assetResourceHandle = ResourceHandle<T>::createResource([this, name]() -> T* {
                auto assetObject = this->getAssetObject(name);
                FF_CONSOLE_LOG("Asset `%s` is being loaded as `%s` via `%s`.", name, assetObject["type"], typeid(T).name());
                T* assetPtr = L::load(*this, assetObject);
                FF_ASSERT(assetPtr != nullptr, "Asset loaders do not currently support returning nullptr.");
                return assetPtr;
            });

            if(cacheInternally) {
                _cache.insert(std::make_pair(key, (IResourceHandle*)new ResourceHandle<T>(assetResourceHandle)));
            }
            return assetResourceHandle;
        }
        return *static_cast<ResourceHandle<T>*>(cacheIt->second);
    }
}

#endif

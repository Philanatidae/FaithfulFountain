/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_RESOURCES_RESOURCE_HANDLE_HPP
#define _FAITHFUL_FOUNTAIN_RESOURCES_RESOURCE_HANDLE_HPP

#include <functional>
#include <unordered_map>
#include <type_traits>

#include <ff/io/Serializer.hpp>

namespace ff {
    namespace _internal {
        struct ResourceHandleInfo {
            void* resourcePtr;
            int refCount;
            std::function<void*()> voidLoadFn;
            std::function<void(void*)> voidDeleteFn;
        };
    }

    struct IResourceHandle {
        virtual ~IResourceHandle() { }
    };

    template<typename T>
    class ResourceHandle : public IResourceHandle {
    template<typename U>
    friend class ResourceHandle;

    public:
        ResourceHandle();
        ResourceHandle(const ResourceHandle<T>& other);
        template<typename K, typename std::enable_if<std::is_convertible<K*, T*>::value, void>::type* = nullptr>
        ResourceHandle(const ResourceHandle<K>& other);

        static ResourceHandle createResource(const std::function<T*()>& loadFn);
        static ResourceHandle createNullResource();

        virtual ~ResourceHandle();

        void reload();

        int getRefCount() const;

        T* get() const;

        T& operator*();
        const T& operator*() const;
        T* operator->();
        const T* operator->() const;

        ResourceHandle<T>& operator=(const ResourceHandle<T>& other);
        ResourceHandle<T>& operator=(ResourceHandle<T>& other);
        template<typename K, typename std::enable_if<std::is_convertible<K*, T*>::value, void>::type* = nullptr>
        ResourceHandle<T>& operator=(const ResourceHandle<K>& other);
        template<typename K, typename std::enable_if<std::is_convertible<K*, T*>::value, void>::type* = nullptr>
        ResourceHandle<T>& operator=(ResourceHandle<K>& other);
        bool operator==(const ResourceHandle<T>& other) const;
        bool operator!=(const ResourceHandle<T>& other) const;
        bool operator==(void* const& other) const;
        bool operator!=(void* const& other) const;

    private:
        _internal::ResourceHandleInfo* _handleInfo;

        ResourceHandle(_internal::ResourceHandleInfo* const& handleInfo);

        void incrementHandle();
        void decrementHandle();
    };

    template<typename T>
    struct CustomSerializer<ResourceHandle<T>> {
        static void serialize(Serializer* serializer,
            char const* const label,
            ResourceHandle<T>& x) {
            // @todo Remove pointer serialization
        }
    };

    template<typename T>
    ResourceHandle<T>::ResourceHandle()
        :ResourceHandle(ResourceHandle<T>::createNullResource()) {
    }
    template<typename T>
    ResourceHandle<T>::ResourceHandle(const ResourceHandle<T>& other)
        :_handleInfo(other._handleInfo) {
        incrementHandle();
    }
    template<typename T>
    template<typename K, typename std::enable_if<std::is_convertible<K*, T*>::value, void>::type*>
    ResourceHandle<T>::ResourceHandle(const ResourceHandle<K>& other)
        :_handleInfo(other._handleInfo) {
        incrementHandle();
    }

    template<typename T>
    ResourceHandle<T> ResourceHandle<T>::createResource(const std::function<T*()>& loadFn) {
        _internal::ResourceHandleInfo* handleInfoPtr = new _internal::ResourceHandleInfo;
        handleInfoPtr->refCount = 0;
        handleInfoPtr->resourcePtr = loadFn();
        handleInfoPtr->voidLoadFn = [loadFn]() -> void* {
            return loadFn();
        };
        handleInfoPtr->voidDeleteFn = [](void* voidResourcePtr) -> void {
            T* resourcePtr = static_cast<T*>(voidResourcePtr);
            delete resourcePtr;
        };
        
        return ResourceHandle<T>(handleInfoPtr);
    }
    template<typename T>
    ResourceHandle<T> ResourceHandle<T>::createNullResource() {
        return ResourceHandle<T>::createResource([]() -> T* {
            return nullptr;
        });
    }

    template<typename T>
    ResourceHandle<T>::~ResourceHandle() {
        decrementHandle();
    }

    template<typename T>
    void ResourceHandle<T>::reload() {
        if(_handleInfo->resourcePtr != nullptr) {
            _handleInfo->voidDeleteFn(_handleInfo->resourcePtr);
        }
        _handleInfo->resourcePtr = _handleInfo->voidLoadFn();
    }

    template<typename T>
    int ResourceHandle<T>::getRefCount() const {
        return _handleInfo->refCount;
    }

    template<typename T>
    T* ResourceHandle<T>::get() const {
        return static_cast<T*>(_handleInfo->resourcePtr);
    }

    template<typename T>
    T& ResourceHandle<T>::operator*() {
        return *get();
    }
    template<typename T>
    const T& ResourceHandle<T>::operator*() const {
        return *get();
    }
    template<typename T>
    T* ResourceHandle<T>::operator->() {
        return get();
    }
    template<typename T>
    const T* ResourceHandle<T>::operator->() const {
        return get();
    }

    template<typename T>
    ResourceHandle<T>& ResourceHandle<T>::operator=(const ResourceHandle<T>& other) {
        decrementHandle();
        _handleInfo = other._handleInfo;
        incrementHandle();

        return *this;
    }
    template<typename T>
    ResourceHandle<T>& ResourceHandle<T>::operator=(ResourceHandle<T>& other) {
        decrementHandle();
        _handleInfo = other._handleInfo;
        incrementHandle();

        return *this;
    }
    template<typename T>
    template<typename K, typename std::enable_if<std::is_convertible<K*, T*>::value, void>::type*>
    ResourceHandle<T>& ResourceHandle<T>::operator=(const ResourceHandle<K>& other) {
        decrementHandle();
        _handleInfo = other._handleInfo;
        incrementHandle();

        return *this;
    }
    template<typename T>
    template<typename K, typename std::enable_if<std::is_convertible<K*, T*>::value, void>::type*>
    ResourceHandle<T>& ResourceHandle<T>::operator=(ResourceHandle<K>& other) {
        decrementHandle();
        _handleInfo = other._handleInfo;
        incrementHandle();

        return *this;
    }
    template<typename T>
    bool ResourceHandle<T>::operator==(const ResourceHandle<T>& other) const {
        // Just checking the pointer value to the handle, since handle is shared
        return _handleInfo == other._handleInfo;
    }
    template<typename T>
    bool ResourceHandle<T>::operator!=(const ResourceHandle<T>& other) const {
        return !(*this == other);
    }

    template<typename T>
    bool ResourceHandle<T>::operator==(void* const& other) const {
        return _handleInfo->resourcePtr == other;
    }
    template<typename T>
    bool ResourceHandle<T>::operator!=(void* const& other) const {
        return !(*this == other);
    }

    template<typename T>
    ResourceHandle<T>::ResourceHandle(_internal::ResourceHandleInfo* const& handleInfo)
        :_handleInfo(handleInfo) {
        incrementHandle();
    }

    template<typename T>
    void ResourceHandle<T>::incrementHandle() {
        _handleInfo->refCount++;
    }
    template<typename T>
    void ResourceHandle<T>::decrementHandle() {
        _handleInfo->refCount--;
        if(getRefCount() == 0) {
            // Nobody owns the handle, we need to destroy it
            if(_handleInfo->resourcePtr != nullptr) {
                _handleInfo->voidDeleteFn(_handleInfo->resourcePtr);
            }
            delete _handleInfo;
        }
    }
}

#endif

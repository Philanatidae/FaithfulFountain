/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_UTIL_MEMORY_HPP
#define _FAITHFUL_FOUNTAIN_UTIL_MEMORY_HPP

#include <functional>
#include <unordered_map>
#include <ff/Console.hpp>

namespace ff {
    /* #region unique_ptr */
    template<typename T>
    class unique_ptr final {
    public:
        unique_ptr<T>(T* ptr = nullptr, std::function<void(T*)> deleter = nullptr);
        unique_ptr<T>(const unique_ptr<T>&) = delete;
        unique_ptr<T>(unique_ptr<T>&& other) noexcept;
        ~unique_ptr<T>();

        T* get() const;

        void reset();
        T* release();

        T& operator*();
        const T& operator*() const;
        T* operator->();
        const T* operator->() const;

        bool operator==(T const * const& other) const;
        bool operator!=(T const * const& other) const;
        bool operator==(const unique_ptr<T>& other) const;
        bool operator!=(const unique_ptr<T>& other) const;

        explicit operator bool() const;

    private:
        T* _ptr;
        std::function<void(T*)> _deleter;
    };

    template<typename T>
    unique_ptr<T>::unique_ptr(T* ptr, std::function<void(T*)> deleter)
        :_ptr(ptr),_deleter(deleter) {
    }
    template<typename T>
    unique_ptr<T>::unique_ptr(unique_ptr<T>&& other) noexcept {
        _ptr = nullptr;
        reset();
        _ptr = std::exchange(other._ptr, nullptr);
        _deleter = std::exchange(other._deleter, nullptr);
    }
    template<typename T>
    unique_ptr<T>::~unique_ptr() {
        reset();
    }
    
    template<typename T>
    T* unique_ptr<T>::get() const {
        return _ptr;
    }

    template<typename T>
    void unique_ptr<T>::reset() {
        if(_ptr) {
            if(_deleter) {
                _deleter(_ptr);
            } else {
                delete _ptr;
                _ptr = nullptr;
            }
        }
    }
    template<typename T>
    T* unique_ptr<T>::release() {
        T* ptr = _ptr;
        _ptr = nullptr;
        _deleter = nullptr;
        return ptr;
    }

    template<typename T>
    T& unique_ptr<T>::operator*() {
        return *_ptr;
    }
    template<typename T>
    const T& unique_ptr<T>::operator*() const {
        return *_ptr;
    }
    template<typename T>
    T* unique_ptr<T>::operator->() {
        return _ptr;
    }
    template<typename T>
    const T* unique_ptr<T>::operator->() const {
        return _ptr;
    }

    template<typename T>
    bool unique_ptr<T>::operator==(T const * const& other) const {
        return _ptr == other;
    }
    template<typename T>
    bool unique_ptr<T>::operator!=(T const * const& other) const {
        return !(*this == other);
    }
    template<typename T>
    bool unique_ptr<T>::operator==(const unique_ptr<T>& other) const {
        return _ptr == other._ptr;
    }
    template<typename T>
    bool unique_ptr<T>::operator!=(const unique_ptr<T>& other) const {
        return !(*this == other);
    }

    template<typename T>
    unique_ptr<T>::operator bool() const {
        return _ptr;
    }

    template<typename T, typename... Args>
    unique_ptr<T> make_unique(Args... args) {
        return unique_ptr<T>(new T(args...));
    }
    /* #endregion */

    /* #region shared_ptr */
    template<typename T>
    class weak_ptr;

    template<typename T>
    class shared_ptr final {
    friend class weak_ptr<T>;

    public:
        shared_ptr<T>(T* ptr = nullptr, std::function<void(T*)> deleter = nullptr);
        shared_ptr<T>(const shared_ptr<T>& other);
        shared_ptr<T>(shared_ptr<T>& other);
        ~shared_ptr<T>();

        int use_count() const;

        T* get() const;

        void reset(T* const& ptr = nullptr);

        T& operator*();
        const T& operator*() const;
        T* operator->();
        const T* operator->() const;

        shared_ptr<T>& operator=(const shared_ptr<T>& other);
        shared_ptr<T>& operator=(shared_ptr<T>& other);
        bool operator==(T const * const& other) const;
        bool operator!=(T const * const& other) const;
        bool operator==(const shared_ptr<T>& other) const;
        bool operator!=(const shared_ptr<T>& other) const;

        explicit operator bool() const;

    private:
        void** _ptrPtr = nullptr;

        void increaseRefCount();
        void decreaseRefCount();
    };

    namespace _internal {
        struct CountDeleter {
            int count;
            int totalCount;
            std::function<void(void*)> deleter;
        };
        std::unordered_map<void**, CountDeleter>& getRefCountMap();
    }

    template<typename T>
    shared_ptr<T>::shared_ptr(T* ptr, std::function<void(T*)> deleter) {
        // @note shared_ptr assumes that ptrs are only passed to be owned once.
        _ptrPtr = new (void*)((void*)ptr);
        std::function<void(void*)> deleterWrapper = nullptr;
        if(deleter) {
            deleterWrapper = [deleter](void* ptr) -> void { T* castPtr = static_cast<T*>(ptr); deleter(castPtr); };
        }
        _internal::getRefCountMap().insert(std::make_pair(_ptrPtr,
            _internal::CountDeleter { 1, 1, deleterWrapper }));
    }
    template<typename T>
    shared_ptr<T>::shared_ptr(const shared_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefCount();
    }
    template<typename T>
    shared_ptr<T>::shared_ptr(shared_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefCount();
    }
    template<typename T>
    shared_ptr<T>::~shared_ptr() {
        decreaseRefCount();
    }

    template<typename T>
    int shared_ptr<T>::use_count() const {
        auto it = _internal::getRefCountMap().find(_ptrPtr);
        return it->second.count;
    }
    
    template<typename T>
    T* shared_ptr<T>::get() const {
        return static_cast<T*>(*_ptrPtr);
    }

    template<typename T>
    void shared_ptr<T>::reset(T* const& ptr) {
        T* oldPtr = static_cast<T*>(*_ptrPtr);
        auto it = _internal::getRefCountMap().find(_ptrPtr);
        if(it->second.deleter) {
            it->second.deleter(oldPtr);
        } else {
            delete oldPtr;
        }
        *_ptrPtr = ptr;
    }

    template<typename T>
    T& shared_ptr<T>::operator*() {
        return *static_cast<T*>(*_ptrPtr);
    }
    template<typename T>
    const T& shared_ptr<T>::operator*() const {
        return *static_cast<T*>(*_ptrPtr);
    }
    template<typename T>
    T* shared_ptr<T>::operator->() {
        return static_cast<T*>(*_ptrPtr);
    }
    template<typename T>
    const T* shared_ptr<T>::operator->() const {
        return static_cast<T*>(*_ptrPtr);
    }

    template<typename T>
    shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefCount();

        return *this;
    }
    template<typename T>
    shared_ptr<T>& shared_ptr<T>::operator=(shared_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefCount();

        return *this;
    }

    template<typename T>
    bool shared_ptr<T>::operator==(T const * const& other) const {
        return *_ptrPtr == other;
    }
    template<typename T>
    bool shared_ptr<T>::operator!=(T const * const& other) const {
        return !(*this == other);
    }
    template<typename T>
    bool shared_ptr<T>::operator==(const shared_ptr<T>& other) const {
        return _ptrPtr == other._ptrPtr;
    }
    template<typename T>
    bool shared_ptr<T>::operator!=(const shared_ptr<T>& other) const {
        return !(*this == other);
    }

    template<typename T>
    shared_ptr<T>::operator bool() const {
        return *_ptrPtr;
    }
    
    template<typename T>
    void shared_ptr<T>::increaseRefCount() {
        auto it = _internal::getRefCountMap().find(_ptrPtr);
        FF_ASSERT(it != _internal::getRefCountMap().end(), "shared_ptr is invalid.");
        it->second.count++;
        it->second.totalCount++;
    }
    template<typename T>
    void shared_ptr<T>::decreaseRefCount() {
        auto it = _internal::getRefCountMap().find(_ptrPtr);
        FF_ASSERT(it != _internal::getRefCountMap().end(), "shared_ptr is invalid.");
        if(--it->second.count == 0) {
            reset();
        }
        if(--it->second.totalCount == 0) {
            delete _ptrPtr;
            _internal::getRefCountMap().erase(_ptrPtr);
        }
    }

    template<typename T, typename... Args>
    shared_ptr<T> make_shared(Args... args) {
        return shared_ptr<T>(new T(args...));
    }
    /* #endregion */

    /* #region weak_ptr */
    template<typename T>
    class weak_ptr final {
    public:
        weak_ptr<T>();
        weak_ptr<T>(const shared_ptr<T>& other);
        weak_ptr<T>(const weak_ptr<T>& other);
        weak_ptr<T>(weak_ptr<T>& other);
        weak_ptr<T>(shared_ptr<T>& other);
        ~weak_ptr<T>();

        int use_count() const;

        bool expired() const;
        shared_ptr<T> lock();

        explicit operator bool() const;

        weak_ptr<T>& operator=(const weak_ptr<T>& other);
        weak_ptr<T>& operator=(weak_ptr<T>& other);
        weak_ptr<T>& operator=(const shared_ptr<T>& other);
        weak_ptr<T>& operator=(shared_ptr<T>& other);

    private:
        void** _ptrPtr = nullptr;

        void increaseRefTotalCount();
        void decreaseRefTotalCount();
    };

    template<typename T>
    weak_ptr<T>::weak_ptr()
        :_ptrPtr(nullptr) {
    }
    template<typename T>
    weak_ptr<T>::weak_ptr(const shared_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefTotalCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefTotalCount();
    }
    template<typename T>
    weak_ptr<T>::weak_ptr(const weak_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefTotalCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefTotalCount();
    }
    template<typename T>
    weak_ptr<T>::weak_ptr(weak_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefTotalCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefTotalCount();
    }
    template<typename T>
    weak_ptr<T>::weak_ptr(shared_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefTotalCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefTotalCount();
    }
    template<typename T>
    weak_ptr<T>::~weak_ptr() {
        decreaseRefTotalCount();
    }

    template<typename T>
    int weak_ptr<T>::use_count() const {
        auto it = _internal::getRefCountMap().find(_ptrPtr);
        return it->second.count;
    }

    template<typename T>
    bool weak_ptr<T>::expired() const {
        return _ptrPtr == nullptr || use_count() == 0;
    }
    template<typename T>
    shared_ptr<T> weak_ptr<T>::lock() {
        shared_ptr<T> sharedPtr;
        if(!expired()) {
            sharedPtr.decreaseRefCount();
            sharedPtr._ptrPtr = _ptrPtr;
            sharedPtr.increaseRefCount();
        }
        return sharedPtr;
    }

    template<typename T>
    weak_ptr<T>::operator bool() const {
        return !expired();
    }

    template<typename T>
    weak_ptr<T>& weak_ptr<T>::operator=(const weak_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefTotalCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefTotalCount();

        return *this;
    }
    template<typename T>
    weak_ptr<T>& weak_ptr<T>::operator=(weak_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefTotalCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefTotalCount();
        
        return *this;
    }
    template<typename T>
    weak_ptr<T>& weak_ptr<T>::operator=(const shared_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefTotalCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefTotalCount();

        return *this;
    }
    template<typename T>
    weak_ptr<T>& weak_ptr<T>::operator=(shared_ptr<T>& other) {
        if(_ptrPtr) {
            decreaseRefTotalCount();
        }
        _ptrPtr = other._ptrPtr;
        increaseRefTotalCount();

        return *this;
    }

    template<typename T>
    void weak_ptr<T>::increaseRefTotalCount() {
        if(_ptrPtr) {
            auto it = _internal::getRefCountMap().find(_ptrPtr);
            FF_ASSERT(it != _internal::getRefCountMap().end(), "weak_ptr is invalid.");
            it->second.totalCount++;
        }
    }
    template<typename T>
    void weak_ptr<T>::decreaseRefTotalCount() {
        if(_ptrPtr) {
            auto it = _internal::getRefCountMap().find(_ptrPtr);
            FF_ASSERT(it != _internal::getRefCountMap().end(), "weak_ptr is invalid.");
            if(--it->second.totalCount == 0) {
                delete _ptrPtr;
                _internal::getRefCountMap().erase(_ptrPtr);
            }
        }
    }
    /* #endregion weak_ptr */
}

#endif

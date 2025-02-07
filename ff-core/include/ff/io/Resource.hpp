/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_IO_RESOURCE_HPP
#define _FAITHFUL_FOUNTAIN_IO_RESOURCE_HPP

#include <ff/io/Reloadable.hpp>

#include <functional>
#include <unordered_map>

namespace ff {
    template<typename T>
    class Resource: public Reloadable {
    public:
        Resource(const std::function<void()>& reloadResourceFn, const bool& reloadImmediately = true);
        virtual ~Resource();

        bool valid();
        T& getResource();

    private:
        T** _resource;
        std::function<T*()> _reloadResourceFn;

        static std::unordered_map<T**, int>& getRefCountMap();

    protected:
        void onReload() override;
        void onInvalidate() override;
    };
    
    template<typename T>
    Resource<T>::Resource(const std::function<void()>& reloadResourceFn, const bool& reloadImmediately) {
        _resource = new T*;

        if(reloadImmediately) {
            
        }
    }
    template<typename T>
    Resource<T>::~Resource() {
        delete _resource;
    }

    template<typename T>
    bool Resource<T>::valid() {
    }
    template<typename T>
    T& Resource<T>::getResource() {
    }

    template<typename T>
    std::unordered_map<T**, int>& Resource<T>::getRefCountMap() {
        static std::unordered_map<T**, int> map;
        return map;
    }

    template<typename T>
    void Resource<T>::onReload() {
    }
    template<typename T>
    void Resource<T>::onInvalidate() {
    }
}

#endif

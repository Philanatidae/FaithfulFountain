/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/CameraManager.hpp>

namespace ff {

CameraManager::CameraManager() {
}

bool CameraManager::addPCamera(std::string const& name,
    PerspectiveCamera* const& cam) {
    if(hasCamera(name)) {
        delete cam;
        return false;
    }
    _cameras.insert(std::make_pair(name, cam));
    return true;
}
bool CameraManager::addOCamera(std::string const& name,
    OrthographicCamera* const& cam) {
    if(hasCamera(name)) {
        delete cam;
        return false;
    }
    _cameras.insert(std::make_pair(name, cam));
    return true;
}

bool CameraManager::removeCamera(std::string const& name) {
    if(!hasCamera(name)) {
        return false;
    }
    _cameras.erase(name);
    return true;
}
bool CameraManager::hasCamera(std::string const& name) const {
    return _cameras.find(name) != _cameras.end();
}
bool CameraManager::isPCamera(std::string const& name) const {
    return getPCamera(name) != nullptr;
}
bool CameraManager::isOCamera(std::string const& name) const {
    return getOCamera(name) != nullptr;
}

PerspectiveCamera const* CameraManager::getPCamera(std::string const& name) const {
    if(!hasCamera(name)) {
        return nullptr;
    }
    return dynamic_cast<PerspectiveCamera const*>(_cameras.at(name).get());
}
PerspectiveCamera* CameraManager::getPCamera(std::string const& name) {
    if(!hasCamera(name)) {
        return nullptr;
    }
    return dynamic_cast<PerspectiveCamera*>(_cameras.at(name).get());
}
OrthographicCamera const* CameraManager::getOCamera(std::string const& name) const {
    if(!hasCamera(name)) {
        return nullptr;
    }
    return dynamic_cast<OrthographicCamera const*>(_cameras.at(name).get());
}
OrthographicCamera* CameraManager::getOCamera(std::string const& name) {
    if(!hasCamera(name)) {
        return nullptr;
    }
    return dynamic_cast<OrthographicCamera*>(_cameras.at(name).get());
}

Camera const* CameraManager::getCamera(std::string const& name) const {
    if(!hasCamera(name)) {
        return nullptr;
    }
    return _cameras.at(name).get();
}
Camera* CameraManager::getCamera(std::string const& name) {
    if(!hasCamera(name)) {
        return nullptr;
    }
    return _cameras.at(name).get();
}

std::vector<std::string> CameraManager::getCameras() const {
    std::vector<std::string> names;
    for(auto const& cams : _cameras) {
        names.push_back(cams.first);
    }
    return names;
}

}


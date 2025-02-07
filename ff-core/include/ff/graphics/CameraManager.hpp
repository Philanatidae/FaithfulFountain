/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_GRAPHICS_CAMERA_MANAGER_HPP
#define _FF_GRAPHICS_CAMERA_MANAGER_HPP

#include <ff/graphics/Camera.hpp>
#include <ff/graphics/PerspectiveCamera.hpp>
#include <ff/graphics/OrthographicCamera.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ff {

class CameraManager final {
public:
    CameraManager();
    ~CameraManager() = default;

    bool addPCamera(std::string const& name,
        PerspectiveCamera* const& cam);
    bool addOCamera(std::string const& name,
        OrthographicCamera* const& cam);

    bool removeCamera(std::string const& name);
    bool hasCamera(std::string const& name) const;
    bool isPCamera(std::string const& name) const;
    bool isOCamera(std::string const& name) const;

    PerspectiveCamera const* getPCamera(std::string const& name) const;
    PerspectiveCamera* getPCamera(std::string const& name);
    OrthographicCamera const* getOCamera(std::string const& name) const;
    OrthographicCamera* getOCamera(std::string const& name);

    Camera const* getCamera(std::string const& name) const;
    Camera* getCamera(std::string const& name);

    std::vector<std::string> getCameras() const;

private:
    std::unordered_map<std::string, std::unique_ptr<Camera>> _cameras;
};

}

#endif


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SDL2_WINDOW_ASPECT_RATIO_MAINTAINER_DIRECTOR_HPP
#define _FAITHFUL_FOUNTAIN_SDL2_WINDOW_ASPECT_RATIO_MAINTAINER_DIRECTOR_HPP

#include <ff/processes/Process.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/events/ResizeEvent.hpp>

namespace ff {

class DesktopWindowAspectRatioMaintainerDirector : public Process,
    public EventListener<ResizeEvent> {
public:
    DesktopWindowAspectRatioMaintainerDirector(const float& ratio);
    DesktopWindowAspectRatioMaintainerDirector(const float& minRatio, const float& maxRatio);
    virtual ~DesktopWindowAspectRatioMaintainerDirector() = default;

    bool processEvent(ResizeEvent const &message) override;

protected:
    void onInitialize() override;

private:
    float _minRatio;
    float _maxRatio;
    glm::ivec2 _previousAdjustedBounds;
};

}

#endif

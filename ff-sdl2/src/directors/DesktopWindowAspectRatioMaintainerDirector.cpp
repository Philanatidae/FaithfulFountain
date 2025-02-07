/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-sdl2/directors/DesktopWindowAspectRatioMaintainerDirector.hpp>

#include <ff-sdl2/commands/ResizeWindowCmd.hpp>

#include <ff/Locator.hpp>

namespace ff {
    DesktopWindowAspectRatioMaintainerDirector::DesktopWindowAspectRatioMaintainerDirector(const float& ratio)
        :_minRatio(ratio),_maxRatio(-1),
        _previousAdjustedBounds(-1, -1) {
    }
    DesktopWindowAspectRatioMaintainerDirector::DesktopWindowAspectRatioMaintainerDirector(const float& minRatio, const float& maxRatio)
        :_minRatio(minRatio),_maxRatio(maxRatio),
        _previousAdjustedBounds(-1, -1) {
    }

    bool DesktopWindowAspectRatioMaintainerDirector::processEvent(ResizeEvent const &message) {
        // @todo Check if fullscreen; if so, do not adjust aspect ratio.

        if(_previousAdjustedBounds.x == message.width
            && _previousAdjustedBounds.y == message.height) {
            return false;
        }
        
        float newAspectRatio = (float)message.width / (float)message.height;
        glm::ivec2 newBounds(-1, -1);
        if(newAspectRatio < _minRatio) {
            // Width is too short. Height dominates.
            newBounds.y = message.height;
            newBounds.x = (int)(newBounds.y * _minRatio);
        }
        // If max ratio is less than 0, this indicates
        // that there is only one aspect ratio allowed,
        // which is determined by _minRatio.
        if(_maxRatio < 0) {
            if(newAspectRatio > _minRatio) {
                // Height is too short. Width dominates.
                newBounds.x = message.width;
                newBounds.y = (int)(newBounds.x / _minRatio);
            }
        } else if(newAspectRatio > _maxRatio) {
            // Height is too short. Width dominates.
            newBounds.x = message.width;
            newBounds.y = (int)(newBounds.x / _maxRatio);
        }
        _previousAdjustedBounds = newBounds;
        Locator::getMessageBus().dispatch<ResizeWindowCmd>(newBounds.x, newBounds.y);

        return false;
    }

    void DesktopWindowAspectRatioMaintainerDirector::onInitialize() {
        Locator::getMessageBus().addListener<ResizeEvent>(this);
    }
}

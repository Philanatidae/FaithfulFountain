/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/ISamplerState.hpp>

namespace ff {
    ISamplerState::ISamplerState(SamplerFilter const& minFilter,
        SamplerFilter const& magFilter,
        SamplerAddressMode const& sAddressMode,
        SamplerAddressMode const& tAddressMode,
        SamplerMipFilter const& mipFilter)
        :_minFilter(minFilter),
        _magFilter(magFilter),
        _sAddressMode(sAddressMode),
        _tAddressMode(tAddressMode),
        _mipFilter(mipFilter) {
    }

    SamplerFilter ISamplerState::getMinFilter() const {
        return _minFilter;
    }
    SamplerFilter ISamplerState::getMagFilter() const {
        return _magFilter;
    }
    SamplerMipFilter ISamplerState::getMipFilter() const {
        return _mipFilter;
    }

    SamplerAddressMode ISamplerState::getSAddressMode() const {
        return _sAddressMode;
    }
    SamplerAddressMode ISamplerState::getTAddressMode() const {
        return _tAddressMode;
    }
}

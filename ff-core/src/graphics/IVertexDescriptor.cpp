/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/IVertexDescriptor.hpp>

namespace ff {
    IVertexDescriptor::IVertexDescriptor(VertexDescriptorInfo const& info)
        :_info(info) {
    }

    VertexDescriptorInfo IVertexDescriptor::getDescriptorInfo() const {
        return _info;
    }
}

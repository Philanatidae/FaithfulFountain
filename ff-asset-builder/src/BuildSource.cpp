/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/BuildSource.hpp>

namespace ff {
    BuildSource::BuildSource() {
    }
    BuildSource::~BuildSource() {
    }

    const std::vector<std::filesystem::path> BuildSource::getInputs() const {
        return _inputs;
    }
    void BuildSource::addInput(const std::filesystem::path& input) {
        _inputs.push_back(input);
    }
    void BuildSource::addInputs(const std::vector<std::filesystem::path>& inputs) {
        _inputs.insert(_inputs.end(), inputs.begin(), inputs.end());
    }

    const nlohmann::json& BuildSource::getConfigData() const {
        return _configData;
    }
    void BuildSource::setConfigData(const nlohmann::json& configData) {
        _configData = configData;
    }
}

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/BuildStep.hpp>

namespace ff {
    BuildStep::BuildStep()
        :_built(false) {
    }
    BuildStep::~BuildStep() {
    }

    const bool& BuildStep::isBuilt() const {
        return _built;
    }
    void BuildStep::setBuilt(const bool& built) {
        _built = built;
    }

    const std::vector<std::string>& BuildStep::getDependencies() const {
        return _dependencies;
    }
    void BuildStep::addDependency(const std::string& dependency) {
        _dependencies.push_back(dependency);
    }
    void BuildStep::addDependencies(const std::vector<std::string>& dependencies) {
        _dependencies.insert(_dependencies.end(), dependencies.begin(), dependencies.end());
    }

    const std::vector<std::string>& BuildStep::getSources() const {
        return _sources;
    }
    void BuildStep::addSource(const std::string& source) {
        _sources.push_back(source);
    }
    void BuildStep::addSources(const std::vector<std::string>& sources) {
        _sources.insert(_sources.end(), sources.begin(), sources.end());
    }

    const std::vector<std::filesystem::path>& BuildStep::getInputs() const {
        return _inputs;
    }
    void BuildStep::addInput(const std::filesystem::path& input) {
        _inputs.push_back(input);
    }
    void BuildStep::addInputs(const std::vector<std::filesystem::path>& inputs) {
        _inputs.insert(_inputs.end(), inputs.begin(), inputs.end());
    }

    const nlohmann::json& BuildStep::getConfigData() const {
        return _configData;
    }
    void BuildStep::setConfigData(const nlohmann::json& configData) {
        _configData = configData;
    }
}

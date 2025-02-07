/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/BuildTarget.hpp>

namespace ff {
    BuildTarget::BuildTarget() {
    }
    BuildTarget::~BuildTarget() {
    }

    const std::vector<std::string>& BuildTarget::getDependencies() const {
        return _dependencies;
    }
    void BuildTarget::addDependency(const std::string& dependency) {
        _dependencies.push_back(dependency);
    }
    void BuildTarget::addDependencies(const std::vector<std::string>& dependencies) {
        _dependencies.insert(_dependencies.end(), dependencies.begin(), dependencies.end());
    }

    const std::vector<std::string>& BuildTarget::getSources() const {
        return _sources;
    }
    void BuildTarget::addSource(const std::string& source) {
        _sources.push_back(source);
    }
    void BuildTarget::addSources(const std::vector<std::string>& sources) {
        _sources.insert(_sources.end(), sources.begin(), sources.end());
    }

    const std::vector<std::filesystem::path>& BuildTarget::getInputs() const {
        return _inputs;
    }
    void BuildTarget::addInput(const std::filesystem::path& input) {
        _inputs.push_back(input);
    }
    void BuildTarget::addInputs(const std::vector<std::filesystem::path>& inputs) {
        _inputs.insert(_inputs.end(), inputs.begin(), inputs.end());
    }

    const std::vector<std::string>& BuildTarget::getProducts() const {
        return _products;
    }
    void BuildTarget::addProduct(const std::string& product) {
        _products.push_back(product);
    }
    void BuildTarget::addProducts(const std::vector<std::string>& products) {
        _products.insert(_products.end(), products.begin(), products.end());
    }

    const nlohmann::json& BuildTarget::getConfigData() const {
        return _configData;
    }
    void BuildTarget::setConfigData(const nlohmann::json& configData) {
        _configData = configData;
    }
}

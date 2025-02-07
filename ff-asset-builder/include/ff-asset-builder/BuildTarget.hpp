/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_FF_ASSET_BUILDER_BUILD_TARGET_HPP
#define _FAITHFUL_FOUNTAIN_FF_ASSET_BUILDER_BUILD_TARGET_HPP

#include <string>
#include <vector>
#include <stdint.h>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace ff {
    class AssetBuilder;

    class BuildTarget {
    public:
        BuildTarget();
        virtual ~BuildTarget();

        virtual std::string getType() const = 0;
        virtual std::string getName() const = 0;
        virtual uint32_t getFlags() const = 0;

        virtual std::vector<std::filesystem::path> getOutputs(AssetBuilder* builder) const = 0;

        virtual void build(AssetBuilder* builder) = 0;
        virtual void populateMetadata(nlohmann::json& targetObject) = 0;

        const std::vector<std::string>& getDependencies() const;
        void addDependency(const std::string& dependency);
        void addDependencies(const std::vector<std::string>& dependencies);

        const std::vector<std::string>& getSources() const;
        void addSource(const std::string& source);
        void addSources(const std::vector<std::string>& sources);

        const std::vector<std::filesystem::path>& getInputs() const;
        void addInput(const std::filesystem::path& input);
        void addInputs(const std::vector<std::filesystem::path>& inputs);

        const std::vector<std::string>& getProducts() const;
        void addProduct(const std::string& product);
        void addProducts(const std::vector<std::string>& products);

        const nlohmann::json& getConfigData() const;
        void setConfigData(const nlohmann::json& configData);
    private:
        std::vector<std::string> _dependencies;
        std::vector<std::string> _sources;
        std::vector<std::filesystem::path> _inputs;
        std::vector<std::string> _products;
        nlohmann::json _configData;
    };
}

#endif

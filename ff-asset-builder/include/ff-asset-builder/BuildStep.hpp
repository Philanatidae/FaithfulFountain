/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_BUILD_STEP_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_BUILD_STEP_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace ff {
    class AssetBuilder;

    class BuildStep {
    public:
        BuildStep();
        virtual ~BuildStep();

        virtual std::string getType() const = 0;

        const bool& isBuilt() const;
        void setBuilt(const bool& built);

        virtual void build(AssetBuilder* assetBuilder) = 0;

        const std::vector<std::string>& getDependencies() const;
        void addDependency(const std::string& dependency);
        void addDependencies(const std::vector<std::string>& dependencies);

        const std::vector<std::string>& getSources() const;
        void addSource(const std::string& source);
        void addSources(const std::vector<std::string>& sources);

        const std::vector<std::filesystem::path>& getInputs() const;
        void addInput(const std::filesystem::path& input);
        void addInputs(const std::vector<std::filesystem::path>& inputs);

        const nlohmann::json& getConfigData() const;
        void setConfigData(const nlohmann::json& configData);
    private:
        bool _built;
        std::vector<std::string> _dependencies;
        std::vector<std::string> _sources;
        std::vector<std::filesystem::path> _inputs;
        nlohmann::json _configData;
    };
}

#endif

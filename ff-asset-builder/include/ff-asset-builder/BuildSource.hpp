/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_BUILD_SOURCE_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_BUILD_SOURCE_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace ff {
    class BuildSource {
    public:
        BuildSource();
        virtual ~BuildSource();

        virtual std::string getType() const = 0;

        const std::vector<std::filesystem::path> getInputs() const;
        void addInput(const std::filesystem::path& input);
        void addInputs(const std::vector<std::filesystem::path>& inputs);

        const nlohmann::json& getConfigData() const;
        void setConfigData(const nlohmann::json& configData);
    private:
        std::vector<std::filesystem::path> _inputs;
        nlohmann::json _configData;
    };
}

#endif

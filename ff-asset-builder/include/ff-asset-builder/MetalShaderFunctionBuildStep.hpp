/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_METAL_SHADER_FUNCTION_BUILD_STEP_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_METAL_SHADER_FUNCTION_BUILD_STEP_HPP

#include <ff-asset-builder/BuildStep.hpp>

#include <filesystem>
#include <ff-asset-builder/ShaderFunctionBinding.hpp>

#include <string>
#include <vector>

namespace ff {
    class MetalShaderFunctionBuildStep : public BuildStep {
    public:
        MetalShaderFunctionBuildStep(const std::string& name);
        ~MetalShaderFunctionBuildStep();

        std::string getType() const override;

        void build(AssetBuilder* assetBuilder) override;

        const std::string& getName() const;

        const std::filesystem::path& getAirPath() const;
        const std::vector<ShaderFunctionBinding>& getBindings() const;

    private:
        std::string _name;

        std::filesystem::path _airPath;
        std::vector<ShaderFunctionBinding> _bindings;
    };
}

#endif

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_GL_SHADER_LIBRARY_BUILD_TARGET_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_GL_SHADER_LIBRARY_BUILD_TARGET_HPP

#include <ff-asset-builder/BuildTarget.hpp>

#include <vector>
#include <string>

#include <ff-asset-builder/ShaderDefinition.hpp>
#include <ff-asset-builder/ShaderFunctionBinding.hpp>

namespace ff {
    class GLShaderLibraryBuildTarget : public BuildTarget {
    public:
        GLShaderLibraryBuildTarget(const std::string& name,
            const std::vector<ShaderDefinition>& shaders,
            const bool& isES);
        ~GLShaderLibraryBuildTarget();

        std::string getType() const override;
        std::string getName() const override;
        uint32_t getFlags() const override;

        std::vector<std::filesystem::path> getOutputs(AssetBuilder* builder) const override;

        const std::vector<ShaderDefinition>& getShaders() const;
        const bool& isES() const;

        void build(AssetBuilder* builder) override;
        void populateMetadata(nlohmann::json& targetObject) override;
    
    private:
        std::string _name;
        std::vector<ShaderDefinition> _shaders;
        bool _isES;

        std::vector<nlohmann::json> _shaderMetadata;
    };
}

#endif

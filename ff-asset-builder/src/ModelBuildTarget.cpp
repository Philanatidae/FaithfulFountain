/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/ModelBuildTarget.hpp>

#include <ff-asset-builder/AssetBuilder.hpp>

#include <ff/Console.hpp>

#include <ff/util/OS.hpp>

#include <cgltf.h>
#include <cgltf_write.h>
#include <cstdlib>

namespace ff {

ModelBuildTarget::ModelBuildTarget(std::string const& name,
    std::vector<std::string> const& validates,
    std::vector<std::pair<std::string, std::string>> const& materialAssignments)
    :_name(name),_validates(validates),_materialAssignments(materialAssignments) {    
}

std::string ModelBuildTarget::getType() const {
    return "Model";
}
std::string ModelBuildTarget::getName() const {
    return _name;
}
uint32_t ModelBuildTarget::getFlags() const {
    return 0;
}

std::vector<std::filesystem::path> ModelBuildTarget::getOutputs(AssetBuilder* builder) const {
    return {
        builder->getTargetDir()/_name
    };
}

void ModelBuildTarget::build(AssetBuilder* builder) {
    FF_ASSERT(getInputs().size() == 1, "ModelBuildTarget must have exactly one input.");
    auto& sourcePath = getInputs()[0];

    // @todo Allow .gltf extensions, package into .glb after processing. Not currently supported by the library.
    FF_ASSERT(getFileExtension(sourcePath.string()) == "glb", "Model `%s` must be a .glb file.");

    // Load in the model and do some validation
    cgltf_options options = {};
    std::memset(&options, 0, sizeof(cgltf_options));
    cgltf_data* data = nullptr;

    // Load file
    FF_ASSERT(std::filesystem::exists(sourcePath.string()), "Model file at path does not exist: %s", sourcePath.string());
    FF_ASSERT(cgltf_parse_file(&options, sourcePath.string().c_str(), &data) == cgltf_result_success, "Could not parse GLTF2 file.");
    // FF_ASSERT(cgltf_load_buffers(&options, data, sourcePath.c_str()) == cgltf_result_success, "Could not load buffers from GLTF2 file.");

    // Validations.
    // 1) There should be at least one node.
    FF_ASSERT(data->nodes_count > 0, "Model `%s` must contain at least one node.", getName());

    // 2) There should only be one root node.
    {
        bool foundRoot = false;
        for(int i = 0; i < data->nodes_count; i++) {
            if(data->nodes[i].parent == nullptr) {
                FF_ASSERT(!foundRoot, "Model `%s` must contain only one root node.", getName());
                foundRoot = true;
            }
        }
    }

    // 3) If there are materials, we need to warn that they will be ignored.
    if(data->materials_count > 0) {
        FF_CONSOLE_WARN("Model `%s` contains materials. Materials are ignored in the engine and must be specified and assigned manually via Directory.xml.");
    }

    // 4) Validate meshes, if we are to validate any. A FF mesh
    // is a Node in GLTF terms.
    if(_validates.size() > 0) {
        std::vector<bool> validations;
        validations.resize(_validates.size(), false);
        for(int i = 0; i < data->nodes_count; i++) {
            auto it = std::find(_validates.begin(), _validates.end(), data->nodes[i].name);
            if(it != _validates.end()) {
                validations[it - _validates.begin()] = true;
            }
        }
        if(std::find(validations.begin(), validations.end(), false) != validations.end()) {
            FF_CONSOLE_ERROR("Mesh validation failed. The following meshes are missing:");
            for(int i = 0; i < validations.size(); i++) {
                if(validations[i]) {
                    continue;
                }

                FF_CONSOLE_ERROR("\t - %s", _validates[i]);
            }
            exit(EXIT_FAILURE);
        }
    }

    // Write file in .glb format. This will combine the buffers into a single file,
    // which the engine is expecting.
    // @note Not currently supported by the library.
    copyFileIfNewer(sourcePath, getOutputs(builder)[0]);
    // FF_ASSERT(cgltf_write_file(&options, getOutputs(builder)[0].c_str(), data),
    //     "Could not write `%s`.", getName());

    cgltf_free(data);
}
void ModelBuildTarget::populateMetadata(nlohmann::json& targetObject) {
    targetObject["path"] = _name;
    targetObject["material-assignments"] = nlohmann::json::object();
    for(auto& pair : _materialAssignments) {
        targetObject["material-assignments"][pair.first] = pair.second;
    }
}

}

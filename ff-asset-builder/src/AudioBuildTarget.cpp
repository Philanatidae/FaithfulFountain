/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/AudioBuildTarget.hpp>

#include <ff-asset-builder/AssetBuilder.hpp>

#include <ff/Console.hpp>
#include <ff/util/OS.hpp>

namespace ff {
    AudioBuildTarget::AudioBuildTarget(const std::string& name)
        :_name(name) {
    }
    AudioBuildTarget::~AudioBuildTarget() {
    }

    std::string AudioBuildTarget::getType() const {
        return "Audio";
    }
    std::string AudioBuildTarget::getName() const {
        return _name;
    }
    uint32_t AudioBuildTarget::getFlags() const {
        return 0;
    }

    std::vector<std::filesystem::path> AudioBuildTarget::getOutputs(AssetBuilder* builder) const {
        return {
            builder->getTargetDir()/_name
        };
    }

    void AudioBuildTarget::build(AssetBuilder* builder) {
        FF_ASSERT(getInputs().size() == 1, "AudioBuildTarget must have exactly one input.");
        auto& sourcePath = getInputs()[0];
        copyFileIfNewer(sourcePath, getOutputs(builder)[0]);
    }
    void AudioBuildTarget::populateMetadata(nlohmann::json& targetObject) {
        targetObject["path"] = _name;
    }
}

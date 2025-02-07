/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/AssetBuilder.hpp>

#include <tinyformat/tinyformat.h>
#include <ff/Console.hpp>
#include <ff/CVars.hpp>
#include <ff/util/String.hpp>
#include <optional>
#include <fstream>

#include <ff-asset-builder/RawBuildTarget.hpp>
#include <ff-asset-builder/AudioBuildTarget.hpp>
#include <ff-asset-builder/SimpleTextureBuildTarget.hpp>
#include <ff-asset-builder/TextureAtlasBuildTarget.hpp>
#include <ff-asset-builder/MSDFBitmapFontBuildTarget.hpp>
#include <ff-asset-builder/MaterialBuildTarget.hpp>
#include <ff-asset-builder/ModelBuildTarget.hpp>
#include <ff-asset-builder/MetalShaderLibraryBuildTarget.hpp>
#include <ff-asset-builder/GLShaderLibraryBuildTarget.hpp>

#include <ff-asset-builder/MetalShaderFunctionBuildStep.hpp>
#include <ff-asset-builder/GLShaderFunctionBuildStep.hpp>

#include <ff-asset-builder/TextureBuildSource.hpp>

#include <stdlib.h>

#include <nlohmann/json.hpp>
#include <ff/util/Time.hpp>
#include <ff/util/OS.hpp>

#include <unordered_set>

#include <ff/io/BinaryMemory.hpp>

#include <ff/util/String.hpp>

namespace ff {
    std::string convertPlatformTargetToString(const PlatformTarget& platform) {
        switch (platform) {
        case PlatformTarget::WINDOWS:
            return "Windows";
        case PlatformTarget::MACOS:
            return "macOS";
        case PlatformTarget::ANDROID:
            return "Android";
        case PlatformTarget::IOS:
            return "iOS";
        case PlatformTarget::IOS_SIMULATOR:
            return "iOS Simulator";
        default:
            return "Unknown";
        }
    }
    PlatformTarget convertStringToPlatformTarget(const std::string& platform) {
        // Platform string to enum. Convert `platform` string to lowercase before comparing.
        std::string lowerPlatform = platform;
        std::transform(lowerPlatform.begin(), lowerPlatform.end(), lowerPlatform.begin(), ::tolower);
        if(lowerPlatform == "windows") {
            return PlatformTarget::WINDOWS;
        } else if(lowerPlatform == "macos") {
            return PlatformTarget::MACOS;
        } else if(lowerPlatform == "android") {
            return PlatformTarget::ANDROID;
        } else if(lowerPlatform == "ios") {
            return PlatformTarget::IOS;
        } else if(lowerPlatform == "ios-simulator") {
            return PlatformTarget::IOS_SIMULATOR;
        } else {
            return PlatformTarget::UNKNOWN;
        }
    }
    std::string convertGraphicsTargetToString(const GraphicsTarget& platform) {
        switch (platform) {
        case GraphicsTarget::METAL:
            return "Metal";
        case GraphicsTarget::GL:
            return "GL";
        case GraphicsTarget::GLES:
            return "GLES";
        default:
            return "Unknown";
        }
    }
    GraphicsTarget convertStringToGraphicsTarget(const std::string& graphics) {
        // Graphics string to enum. Convert `graphics` string to lowercase before comparing.
        std::string lowerGraphics = graphics;
        std::transform(lowerGraphics.begin(), lowerGraphics.end(), lowerGraphics.begin(), ::tolower);
        if(lowerGraphics == "metal") {
            return GraphicsTarget::METAL;
        } else if(lowerGraphics == "gl") {
            return GraphicsTarget::GL;
        } else if(lowerGraphics == "gles") {
            return GraphicsTarget::GLES;
        } else {
            return GraphicsTarget::UNKNOWN;
        }
    }

    AssetBuilder::AssetBuilder(const std::filesystem::path& sourceDir,
        const std::filesystem::path& targetDir,
        const PlatformTarget& platform,
        const std::vector<GraphicsTarget>& graphics)
        :_sourceDir(sourceDir),_targetDir(targetDir),
        _objDir(sourceDir/".obj"),_platformTarget(platform),
        _graphicsTargets(graphics),
        _isValidForDistribution(true) {
    }
    AssetBuilder::~AssetBuilder(){
    }

    const std::filesystem::path& AssetBuilder::getSourceDir() const {
        return _sourceDir;
    }
    const std::filesystem::path& AssetBuilder::getObjectDir() const {
        return _objDir;
    }
    const std::filesystem::path& AssetBuilder::getTargetDir() const {
        return _targetDir;
    }

    const PlatformTarget& AssetBuilder::getPlatformTarget() const {
        return _platformTarget;
    }
    const std::vector<GraphicsTarget>& AssetBuilder::getGraphicsTargets() const {
        return _graphicsTargets;
    }

    bool AssetBuilder::isProductionBuild() const {
        return CVars::get<bool>("asset_builder_production_build");
    }
    bool AssetBuilder::isDevelopmentBuild() const {
        return !isProductionBuild();
    }

    std::shared_ptr<BuildTarget> AssetBuilder::addBuildTarget(const std::string& name,
        const std::shared_ptr<BuildTarget>& target) {
        _buildTargets.emplace(name, target);
        return target;
    }
    std::shared_ptr<BuildTarget> AssetBuilder::getBuildTarget(const std::string& name) const {
        auto it = _buildTargets.find(name);
        if(it != _buildTargets.end()) {
            return it->second;
        }
        return nullptr;
    }
    std::shared_ptr<BuildStep> AssetBuilder::addBuildStep(const std::string& name,
        const std::shared_ptr<BuildStep>& target) {
        _buildSteps.emplace(name, target);
        return target;
    }
    std::shared_ptr<BuildSource> AssetBuilder::addBuildSource(const std::string& name,
        const std::shared_ptr<BuildSource>& source) {
        _buildSources.emplace(name, source);
        return source;
    }
    std::shared_ptr<BuildStep> AssetBuilder::buildAndGetBuildStep(const std::string& name) {
        auto it = _buildSteps.find(name);
        if(it == _buildSteps.end()) {
            return nullptr;
        }
        std::shared_ptr<BuildStep> step = it->second;
        for(const auto& dependency : step->getDependencies()) {
            buildAndGetBuildStep(dependency);
        }
        if(!step->isBuilt()) {
            step->build(this);
        }
        step->setBuilt(true);
        return step;
    }
    std::shared_ptr<BuildStep> AssetBuilder::getBuildStep(const std::string& name) const {
        auto it = _buildSteps.find(name);
        if(it == _buildSteps.end()) {
            return nullptr;
        }
        return it->second;
    }
    const std::unordered_map<std::string, std::shared_ptr<BuildSource>> AssetBuilder::getBuildSources() {
        return _buildSources;
    }
    std::shared_ptr<BuildSource> AssetBuilder::getBuildSource(const std::string& name) const {
        auto it = _buildSources.find(name);
        if(it == _buildSources.end()) {
            return nullptr;
        }
        return it->second;
    }

    bool AssetBuilder::runCommand(const std::string& command) {
        int code = system(command.c_str());
        FF_CONSOLE_LOG("Command `%s` completed with return code `%s`.", command, code);
        return code == EXIT_SUCCESS;
    }

    void AssetBuilder::clean() {
        FF_CONSOLE_LOG("Cleaning target directory...");
        if(std::filesystem::exists(getTargetDir())) {
            std::filesystem::remove_all(getTargetDir());
        }
        FF_CONSOLE_LOG("Cleaning object directory...");
        if(std::filesystem::exists(getObjectDir())) {
            std::filesystem::remove_all(getObjectDir());
        }
    }

    void AssetBuilder::prepare() {
        FF_CONSOLE_LOG("Validating inputs...");
        validateInputs();
        FF_CONSOLE_LOG("Parsing Directory.xml...");
        parseDirectory();
    }
    void AssetBuilder::addTargetToBuild(const std::string& name) {
        // Do not add if target is already in build
        if(std::count(_targetsToBuild.begin(), _targetsToBuild.end(), name) > 0) {
            return;
        }
        // Assert an error if the target is not found.
        auto it = _buildTargets.find(name);
        if(it == _buildTargets.end()) {
            FF_CONSOLE_LOG("Error: Target `%s` not found. Check that the target is defined in Directory.xml.", name);
            return;
        }
        FF_CONSOLE_LOG("Adding target `%s` to build.", name);
        _targetsToBuild.push_back(name);
    }
    void AssetBuilder::addTargetsForDeltaBuild() {
        if(!std::filesystem::exists(getObjectDir()/"ff-asset-builder.json")) {
            addAllTargetsToBuild();
            return;
        }
        nlohmann::json cache;
        {
            std::ifstream file(getObjectDir()/"ff-asset-builder.json");
            FF_ASSERT(file.is_open(), "Failed to open ff-asset-builder.json build cache.");
            BinaryMemory memory(file);
            cache = nlohmann::json::parse(memory.toString());
        }
        if(cache.find("targets") == cache.end()
            || cache.find("steps") == cache.end()
            || cache.find("sources") == cache.end()) {
            // Missing info, need to rebuild everything
            addAllTargetsToBuild();
            return;
        }
        for(const auto& targetPair : _buildTargets) {
            const std::string& targetName = targetPair.first;

            // If the target is not in the cache, add it to the build.
            if(cache["targets"].find(targetName) == cache["targets"].end()) {
                addTargetToBuild(targetName);
                continue;
            }

            // If the target's cache does not have metadata, it needs to be rebuilt.
            if(cache["targets"][targetName].find("metadata") == cache["targets"][targetName].end()) {
                addTargetToBuild(targetName);
                continue;
            }

            bool addedToBuild = false;

            // Get all inputs to the target
            std::vector<std::filesystem::path> inputs = getInputsForBuildTarget(targetName);
            timestamp_t lastBuild = cache["targets"][targetName]["lastBuild"];
            // Loop through all the inputs. Convert the date last modified to a Unix timestamp. Compare to the target's "lastBuild" in the cache. If it is newer, add the target to the build.
            for(const auto& input : inputs) {
                FF_ASSERT(std::filesystem::exists(input), "Input for target `%s` does not exist (%s).", targetName, input.string());

                timestamp_t lastModifiedTime = getFileLastModifiedTime(input.string());
                if(lastModifiedTime > lastBuild) {
                    addTargetToBuild(targetName);
                    addedToBuild = true;
                    break;
                }
            }
            if(addedToBuild) {
                continue;
            }

            // If any outputs are missing, add the target to the build.
            auto target = getBuildTarget(targetName);
            std::vector<std::filesystem::path> outputs = target->getOutputs(this);
            for(const auto& output : outputs) {
                if(!std::filesystem::exists(output)) {
                    addTargetToBuild(targetName);
                    addedToBuild = true;
                    break;
                }
            }
            if(addedToBuild) {
                continue;
            }

            // Last, if any of the configuration changed, add the target to the build.
            // if(cache["targets"][targetName]["config"] != target->getConfigData()) {
            if(anyConfigChangedForBuildTarget(targetName, cache)) {
                addTargetToBuild(targetName);
                continue;
            }
        }

        // Loop over targets that have been detected in Directory.xml. Store their metadata from a previous build.
        for(auto it = _buildTargets.begin(); it != _buildTargets.end(); it++) {
            std::string targetName = it->second->getName();
            if(std::find(_targetsToBuild.begin(), _targetsToBuild.end(), targetName) != _targetsToBuild.end()) {
                continue;
            }
            storeTargetMetadataFromCache(targetName, cache);
        }

        if(_targetsToBuild.empty()) {
            // Nothing is being built, but something
            // could have been removed. If so, we'll clean the
            // target directory.
            timestamp_t lastModifiedTime = getFileLastModifiedTime((getSourceDir()/"Directory.xml").string());
            if(lastModifiedTime > cache["lastBuild"]) {
                FF_CONSOLE_LOG("Directory.xml was modified, but no new targets were added. Cleaning target directory...");
                cleanTargetDir();
                FF_CONSOLE_LOG("Writing cache...");
                writeBuilderCache();
                FF_CONSOLE_LOG("Writing INDEX...");
                writeIndex();
            }
        }
    }
    void AssetBuilder::addAllTargetsToBuild() {
        for(const auto& target : _buildTargets) {
            addTargetToBuild(target.first);
        }
    }
    void AssetBuilder::build() {
        if(_targetsToBuild.size() == 0) {
            FF_CONSOLE_LOG("No targets to build. Nothing to do.");
            return;
        }

        if(isProductionBuild()) {
            FF_CONSOLE_LOG("Building assets for production.")
        } else {
            FF_CONSOLE_LOG("Building assets for development.")
        }

        // @todo Intelligently remove outdated files from the target directory.

        FF_CONSOLE_LOG("Building targets...");
        buildTargets();
    }
    void AssetBuilder::buildAll() {
        FF_CONSOLE_LOG("Adding all targets to build...");
        addAllTargetsToBuild();
        build();
    }

    void AssetBuilder::invalidateForDistribution() {
        FF_CONSOLE_WARN("A build step/target invalidated the bundle for distribution.");
        _isValidForDistribution = false;
    }

    void AssetBuilder::validateInputs() {
        FF_ASSERT(std::filesystem::exists(getSourceDir()), "Source directory does not exist: %s", getSourceDir().string().c_str());
        FF_ASSERT(std::filesystem::is_directory(_sourceDir), "Source directory is not a directory: %s", getSourceDir().string().c_str());
        FF_CONSOLE_LOG("Source directory: %s", getSourceDir().string().c_str());
        FF_CONSOLE_LOG("Target directory: %s", getTargetDir().string().c_str());
        if(!std::filesystem::exists(getTargetDir())) {
            FF_CONSOLE_LOG("Target directory does not exist, creating...");
            std::filesystem::create_directory(getTargetDir());
        }
        FF_CONSOLE_LOG("Object directory: %s", getObjectDir().string().c_str());
        if(!std::filesystem::exists(getObjectDir())) {
            FF_CONSOLE_LOG("Object directory does not exist, creating...");
            std::filesystem::create_directory(getObjectDir());
        }

        FF_ASSERT(getPlatformTarget() != PlatformTarget::UNKNOWN, "Valid platform must be specified.");
        FF_CONSOLE_LOG("Platform: %s", convertPlatformTargetToString(getPlatformTarget()).c_str());

        FF_ASSERT(getGraphicsTargets().size() > 0, "No graphics backends specified.");
        for(auto& graphics : getGraphicsTargets()) {
            FF_ASSERT(graphics != GraphicsTarget::UNKNOWN, "Valid graphics backend must be specified.");
            FF_CONSOLE_LOG("Graphics backend: %s", convertGraphicsTargetToString(graphics).c_str());
        }
    }
    pugi::xml_document AssetBuilder::loadDirectory() {
        std::filesystem::path directoryPath = getSourceDir()/"Directory.xml";
        FF_ASSERT(std::filesystem::exists(directoryPath), "`Directory.xml` does not exist in assets path.");

        pugi::xml_document directoryDoc;
        auto directoryDocLoadResult = directoryDoc.load_file(std::filesystem::absolute(directoryPath).string().c_str());
        FF_ASSERT(directoryDocLoadResult, "Could not load `Directory.xml`: %s", directoryDocLoadResult.description());

        pugi::xml_node directoryNode = directoryDoc.child("Directory");
        FF_ASSERT(directoryNode, "`Directory.xml` does not have root <Directory> element.");
        FF_ASSERT(directoryNode.attribute("version"), "<Directory> element does not contain `version`.");
        FF_ASSERT(directoryNode.attribute("version").as_int() == 0, "Incorrect directory version: must be 0.");

        return directoryDoc;
    }
    void AssetBuilder::parseDirectory() {
        pugi::xml_document directoryDoc = loadDirectory();

        for (pugi::xml_node& targetNode : directoryDoc.child("Directory").children()) {
            std::string targetType = targetNode.name();
            FF_ASSERT(targetNode.attribute("name"), "Asset type `%s` does not have `name` attribute.", targetType.c_str());
            std::string targetName = targetNode.attribute("name").as_string();

            nlohmann::json targetConfig = nlohmann::json::object();
            writeResourceConfigToConfigJSON(targetConfig, targetNode);

            if (targetType == "Raw") {
                FF_CONSOLE_LOG("Adding Raw build target `%s`...", targetName);
                FF_ASSERT(targetNode.attribute("path"), "Asset `%s` does not have `path` attribute.", targetName.c_str());
                std::string targetPath = targetNode.attribute("path").as_string();
                auto target = addBuildTarget(targetName, 
                    std::make_shared<RawBuildTarget>(targetName));
                target->addInput(getSourceDir()/targetPath);
                target->setConfigData(targetConfig);
            } else if (targetType == "Audio") {
                FF_CONSOLE_LOG("Adding Audio build target `%s`...", targetName);
                FF_ASSERT(targetNode.attribute("path"), "Asset `%s` does not have `path` attribute.", targetName.c_str());
                std::string targetPath = targetNode.attribute("path").as_string();
                auto target = addBuildTarget(targetName,
                    std::make_shared<AudioBuildTarget>(targetName));
                target->addInput(getSourceDir()/targetPath);
                target->setConfigData(targetConfig);
            } else if(targetType == "Texture") {
                FF_CONSOLE_LOG("Processing Texture `%s`...", targetName);
                FF_ASSERT(targetNode.attribute("path"), "Asset `%s` does not have `path` attribute.", targetName.c_str());
                std::string targetPath = targetNode.attribute("path").as_string();

                std::string targetUsage = "target";
                if(targetNode.attribute("usage")) {
                    targetUsage = targetNode.attribute("usage").as_string();
                }
                bool isAlphaPremultiplied = false;
                if(targetNode.attribute("pre-multiplied-alpha")) {
                    isAlphaPremultiplied = targetNode.attribute("pre-multiplied-alpha").as_bool();
                }

                std::string lowerTargetUsage = targetUsage;
                std::transform(lowerTargetUsage.begin(), lowerTargetUsage.end(), lowerTargetUsage.begin(), ::tolower);
                if(lowerTargetUsage == "target") {
                    bool mipMap = false;
                    if(targetNode.attribute("mip-map")) {
                        mipMap = targetNode.attribute("mip-map").as_bool();
                    }

                    FF_CONSOLE_LOG("Adding Texture `%s` as a build target.", targetName);
                    auto target = addBuildTarget(targetName,
                        std::make_shared<SimpleTextureBuildTarget>(targetName,
                            targetName,
                            isAlphaPremultiplied,
                            mipMap));
                    target->addInput(getSourceDir()/targetPath);
                    target->setConfigData(targetConfig);
                } else if(lowerTargetUsage == "source") {
                    FF_CONSOLE_LOG("Adding Texture `%s` as a build source.", targetName);
                    auto source = addBuildSource(targetName,
                        std::make_shared<TextureBuildSource>(targetName,
                            isAlphaPremultiplied));
                    source->addInput(getSourceDir()/targetPath);
                    source->setConfigData(targetConfig);

                    // Warn about not applicable parameters
                    if(targetNode.attribute("mip-map")) {
                        FF_CONSOLE_WARN("`mip-map` option on `%s` is ignored when usage = \"source\"");
                    }
                } else {
                    FF_CONSOLE_ERROR("Invalid `usage` attribute value for Texture `%s`: %s", targetName, targetUsage);
                }
            } else if(targetType == "Material") {
                FF_CONSOLE_LOG("Processing Material `%s`...", targetName);
                std::string shaderName = "mesh";
                if(targetNode.attribute("shader")) {
                    shaderName = targetNode.attribute("shader").as_string();
                }
                std::string tintColorStr = "255,255,255";
                if(targetNode.attribute("tint-color")) {
                    tintColorStr = targetNode.attribute("tint-color").as_string();
                }
                std::optional<std::string> diffuseTextureName;
                if(targetNode.attribute("diffuse-texture")) {
                    diffuseTextureName = targetNode.attribute("diffuse-texture").as_string();
                }

                // @todo
                int tintR, tintG, tintB;
                std::vector<std::string> tintColorParts = split(tintColorStr, ',');
                FF_ASSERT(tintColorParts.size() == 3, "Tint color must have three components, separated by a comma with no spaces.");
                tintR = std::stoi(tintColorParts[0].c_str());
                FF_ASSERT(tintR >= 0 && tintR <= 255, "R-tint component must be on the range [0, 255].");
                tintG = std::stoi(tintColorParts[1].c_str());
                FF_ASSERT(tintG >= 0 && tintG <= 255, "B-tint component must be on the range [0, 255].");
                tintB = std::stoi(tintColorParts[2].c_str());
                FF_ASSERT(tintB >= 0 && tintB <= 255, "B-tint component must be on the range [0, 255].");

                auto target = addBuildTarget(targetName,
                    std::make_shared<MaterialBuildTarget>(targetName,
                        shaderName,
                        tintR, tintG, tintB,
                        diffuseTextureName));
                if(diffuseTextureName) {
                    // @todo Validate target dependencies
                    // target->addDependency(*diffuseTextureName);
                }
                target->setConfigData(targetConfig);
            } else if(targetType == "Model") {
                FF_CONSOLE_LOG("Processing Model `%s`...", targetName);
                FF_ASSERT(targetNode.attribute("path"), "Model `%s` does not have `path` attribute.", targetName);
                std::string targetPath = targetNode.attribute("path").as_string();

                std::vector<std::string> validates;

                bool validateMeshes = false;
                if(targetNode.attribute("validate-meshes")) {
                    validateMeshes = targetNode.attribute("validate-meshes").as_bool();

                    if(validateMeshes) {
                        for(auto& node : targetNode.children("Validate")) {
                            FF_ASSERT(node.attribute("mesh"), "Validation for Model `%s` does not have `mesh` attribute.", targetName);
                            validates.push_back(node.attribute("mesh").as_string());
                        }
                    }
                }

                std::vector<std::pair<std::string, std::string>> materialAssignments;
                for(auto& node : targetNode.children("Assign")) {
                    FF_ASSERT(node.attribute("mesh"), "Material assignment for Model `%s` does not have a `mesh` attribute.", targetName);
                    std::string meshName = node.attribute("mesh").as_string();
                    FF_ASSERT(node.attribute("material"), "Material assignemnt for mesh `%s` in Model `%s` does not have a `material` attribute.", targetName);
                    std::string materialName = node.attribute("material").as_string();

                    materialAssignments.push_back(std::make_pair(meshName, materialName));
                } 

                auto target = addBuildTarget(targetName,
                    std::make_shared<ModelBuildTarget>(targetName,
                        validates,
                        materialAssignments));
                target->addInput(getSourceDir()/targetPath);
                target->setConfigData(targetConfig);
            } else if(targetType == "TextureAtlas") {
                FF_CONSOLE_LOG("Processing Texture Atlas `%s`...", targetName);
                int border = targetNode.attribute("border").as_int(0);
                int padding = targetNode.attribute("padding").as_int(0);
                bool preMultiplyAlpha = targetNode.attribute("pre-multiply-alpha").as_bool(false);

                std::vector<std::string> textureSourceNames;
                for(pugi::xml_node& sourceNode : targetNode.children("Source")) {
                    FF_ASSERT(sourceNode.attribute("texture"), "Texture Atlas `%s` does not have `texture` attribute.", targetName);
                    std::string sourceName = sourceNode.attribute("texture").as_string();
                    FF_CONSOLE_LOG("Adding `%s` as a source to texture atlas `%s`...", sourceName, targetName);
                    textureSourceNames.push_back(sourceName);
                }

                auto target = addBuildTarget(targetName,
                    std::make_shared<TextureAtlasBuildTarget>(targetName,
                        textureSourceNames,
                        border,
                        padding,
                        preMultiplyAlpha));
                target->addSources(textureSourceNames);
                target->setConfigData(targetConfig);
            } else if(targetType == "BitmapFont") {
                FF_CONSOLE_LOG("Processing BitmapFont `%s`...", targetName);
                FF_ASSERT(targetNode.attribute("path"), "BitmapFont `%s` does not have `path` attribute.", targetName.c_str());
                std::string targetPath = targetNode.attribute("path").as_string();
                FF_ASSERT(targetNode.attribute("type"), "BitmapFont `%s` missing type.", targetName);
                std::string type = targetNode.attribute("type").as_string();

                // @todo Allow packing into a texture atlas. Need to move away from msdf-bmfont-xml so that we have individual textures.
                FF_ASSERT(!targetNode.attribute("texture-usage") ||
                    (targetNode.attribute("texture-usage") && std::strcmp(targetNode.attribute("texture-usage").as_string(), "target") == 0),
                    "BitmapFont `%s` must have `texture-usage` attribute set to `target`.", targetName);

                if(compareCaseInsensitive(type, "msdf")) {
                    FF_ASSERT(targetNode.attribute("msdf-radius"), "BitmapFont `%s` missing msdf-radius.", targetName);
                    int msdfRadius = targetNode.attribute("msdf-radius").as_int();
                    FF_ASSERT(targetNode.attribute("msdf-font-size"), "BitmapFont `%s` missing msdf-font-size.", targetName);
                    int msdfFontSize = targetNode.attribute("msdf-font-size").as_int();

                    auto target = addBuildTarget(targetName,
                        std::make_shared<MSDFBitmapFontBuildTarget>(targetName,
                            msdfFontSize,
                            msdfRadius));
                    target->addInput(getSourceDir()/targetPath);
                    target->setConfigData(targetConfig);
                } else {
                    FF_CONSOLE_FATAL("Invalid BitmapFont type for `%s`: %s.", targetName, type);
                }
            } else if(targetType == "ShaderFunction") {
                FF_CONSOLE_LOG("Processing ShaderFunction `%s`...", targetName);
                FF_ASSERT(targetNode.attribute("path"), "ShaderFunction `%s` does not have `path` attribute.", targetName.c_str());
                std::string targetPath = targetNode.attribute("path").as_string();
                std::string type = targetNode.attribute("type").as_string();
                if(type.size() == 0) {
                    std::filesystem::path path(targetPath);
                    if(compareCaseInsensitive(path.extension().string(), ".vert")) {
                        type = "vertex";
                    } else if(compareCaseInsensitive(path.extension().string(), ".frag")) {
                        type = "fragment";
                    } else {
                        FF_CONSOLE_FATAL("ShaderFunction `%s` has invalid type: %s", targetName, type.c_str());
                    }
                }

                for(const auto& graphicsBackend : _graphicsTargets) {
                    // Append graphics backend name to the end of the function names to prevent name collisions.
                    std::string targetNameWithBackend = targetName + "-" + convertGraphicsTargetToString(graphicsBackend);

                    if(graphicsBackend == GraphicsTarget::METAL) {
                        auto step = addBuildStep(targetNameWithBackend, std::make_shared<MetalShaderFunctionBuildStep>(targetName));
                        step->addInput(getSourceDir()/targetPath);
                        step->setConfigData(targetConfig);
                    } else if(graphicsBackend == GraphicsTarget::GL
                        || graphicsBackend == GraphicsTarget::GLES) {
                        auto step = addBuildStep(targetNameWithBackend, std::make_shared<GLShaderFunctionBuildStep>(targetName,
                            graphicsBackend == GraphicsTarget::GLES));
                        step->addInput(getSourceDir()/targetPath);
                        step->setConfigData(targetConfig);
                    } else {
                        FF_CONSOLE_FATAL("Unsupported graphics backend for ShaderFunction: `%s`.", convertGraphicsTargetToString(graphicsBackend));
                    }
                }
            } else if(targetType == "ShaderLibrary") {
                FF_CONSOLE_LOG("Processing ShaderLibrary `%s`...", targetName);
                FF_ASSERT(targetNode.attribute("name"), "ShaderLibrary `%s` does not have `name` attribute.", targetName.c_str());
                std::string targetName = targetNode.attribute("name").as_string();

                std::vector<ShaderDefinition> metalShaderDefinitions;
                std::vector<std::string> metalLibraryDependencies;
                std::vector<ShaderDefinition> glShaderDefinitions;
                std::vector<std::string> glLibraryDependencies;
                std::vector<ShaderDefinition> glesShaderDefinitions;
                std::vector<std::string> glesLibraryDependencies;
                for(pugi::xml_node& shaderNode : targetNode.children("Shader")) {
                    FF_ASSERT(shaderNode.attribute("name"), "ShaderLibrary `%s` does not have `name` attribute.", targetName.c_str());
                    std::string shaderName = shaderNode.attribute("name").as_string();
                    FF_ASSERT(shaderNode.attribute("vertex-function"), "ShaderLibrary `%s` does not have `vertex-function` attribute.", targetName.c_str());
                    std::string vertexFunction = shaderNode.attribute("vertex-function").as_string();
                    FF_ASSERT(shaderNode.attribute("fragment-function"), "ShaderLibrary `%s` does not have `fragment-function` attribute.", targetName.c_str());
                    std::string fragmentFunction = shaderNode.attribute("fragment-function").as_string();

                    for(const auto& graphicsBackend : _graphicsTargets) {
                        // Append graphics backend name to the end of the function names to prevent name collisions.
                        std::string vertexFunctionName = vertexFunction + "-" + convertGraphicsTargetToString(graphicsBackend);
                        std::string fragmentFunctionName = fragmentFunction + "-" + convertGraphicsTargetToString(graphicsBackend);
                        switch(graphicsBackend) {
                            case GraphicsTarget::METAL:
                                metalShaderDefinitions.push_back(ShaderDefinition(shaderName, vertexFunctionName, fragmentFunctionName));
                                metalLibraryDependencies.push_back(vertexFunctionName);
                                metalLibraryDependencies.push_back(fragmentFunctionName);
                                break;
                            case GraphicsTarget::GL:
                                glShaderDefinitions.push_back(ShaderDefinition(shaderName, vertexFunctionName, fragmentFunctionName));
                                glLibraryDependencies.push_back(vertexFunctionName);
                                glLibraryDependencies.push_back(fragmentFunctionName);
                                break;
                            case GraphicsTarget::GLES:
                                glesShaderDefinitions.push_back(ShaderDefinition(shaderName, vertexFunctionName, fragmentFunctionName));
                                glesLibraryDependencies.push_back(vertexFunctionName);
                                glesLibraryDependencies.push_back(fragmentFunctionName);
                                break;
                            default:
                                FF_CONSOLE_FATAL("Unsupported graphics backend for ShaderLibrary `%s`: `%s`.", targetName, convertGraphicsTargetToString(graphicsBackend));
                        }
                    }
                }
                
                for(const auto& graphicsBackend : _graphicsTargets) {
                    if(graphicsBackend == GraphicsTarget::METAL) {
                        auto target = std::make_shared<MetalShaderLibraryBuildTarget>(targetName + "_metal",
                            metalShaderDefinitions);
                        target->addDependencies(metalLibraryDependencies);
                        target->setConfigData(targetConfig);
                        addBuildTarget(targetName + "_metal", target);
                    } else if(graphicsBackend == GraphicsTarget::GL) {
                        auto target = std::make_shared<GLShaderLibraryBuildTarget>(targetName + "_opengl",
                            glShaderDefinitions,
                            false);
                        target->addDependencies(glLibraryDependencies);
                        target->setConfigData(targetConfig);
                        addBuildTarget(targetName + "_opengl", target);
                    } else if(graphicsBackend == GraphicsTarget::GLES) {
                        auto target = std::make_shared<GLShaderLibraryBuildTarget>(targetName + "_opengl_es",
                            glesShaderDefinitions,
                            true);
                        target->addDependencies(glesLibraryDependencies);
                        target->setConfigData(targetConfig);
                        addBuildTarget(targetName + "_opengl_es", target);
                    } else {
                        FF_CONSOLE_FATAL("Unsupported graphics backend for ShaderLibrary: `%s`.", convertGraphicsTargetToString(graphicsBackend));
                    }
                }
            } else {
                FF_CONSOLE_LOG("Unknown/unsupported build target type `%s`.", targetType);
            }
        }
    }
    void AssetBuilder::storeTargetMetadataFromCache(const std::string& targetName,
        const nlohmann::json& cache) {
        FF_ASSERT(_targetMetadata.find(targetName) == _targetMetadata.end(), "Metadata for target `%s` already exists.", targetName.c_str());
        _targetMetadata.emplace(targetName, cache["targets"][targetName]["metadata"]);
        // Loop over products, add their metadata.
        for(const auto& product : cache["targets"][targetName]["products"]) {
            storeTargetMetadataFromCache(product, cache);
        }
    }
    void AssetBuilder::buildTargets() {
        for(int i = 0; i < _targetsToBuild.size(); i++) // For loop allows for targets to add other targets during build
        {
            auto target = getBuildTarget(_targetsToBuild[i]);

            for(const auto& dependency : target->getDependencies()) {
                buildAndGetBuildStep(dependency);
            }
            FF_CONSOLE_LOG("Building target `%s`...", target->getName());
            target->build(this);
            FF_CONSOLE_LOG("Writing metadata for target `%s`...", target->getName());
            auto targetMetadata = nlohmann::json::object();
            targetMetadata["type"] = target->getType();
            targetMetadata["name"] = target->getName();
            target->populateMetadata(targetMetadata);
            _targetMetadata.emplace(target->getName(), targetMetadata);
        }
        FF_CONSOLE_LOG("Writing INDEX...");
        writeIndex();
        FF_CONSOLE_LOG("Writing cache...");
        writeBuilderCache();
        FF_CONSOLE_LOG("Cleaning target directory...");
        cleanTargetDir();
        FF_CONSOLE_LOG("Done.");
    }
    void AssetBuilder::writeIndex() {
        nlohmann::json index;
        index["version"] = BUNDLE_VERSION;
        index["valid"] = _isValidForDistribution;
        index["platform"] = convertPlatformTargetToString(getPlatformTarget());
        std::vector<std::string> targetsAddedToIndex;
        for(const auto& metadataPair : _targetMetadata) {
            index[metadataPair.first] = metadataPair.second;
        }

        std::filesystem::path indexPath = _targetDir/"INDEX";
        {
            std::ofstream indexFile(indexPath.string());
            indexFile << index.dump(4) << std::endl;
        }
    }
    void AssetBuilder::cleanTargetDir() {
        std::vector<std::filesystem::path> filesToRemove;
        for(auto& entry : std::filesystem::directory_iterator(_targetDir)) {
            if(entry.path().filename().string() != "INDEX") {
                filesToRemove.push_back(entry.path());
            }
        }
        for(const auto& targetPair : _buildTargets) {
            for(const auto& output : targetPair.second->getOutputs(this)) {
                for(auto it = filesToRemove.begin(); it != filesToRemove.end(); ++it) {
                    if(*it == output) {
                        filesToRemove.erase(it);
                        break;
                    }
                }
            }
        }
        for(const auto& file : filesToRemove) {
            std::filesystem::remove(file);
        }
    }
    void AssetBuilder::writeBuilderCache() {
        nlohmann::json cache;
        cache["platform"] = convertPlatformTargetToString(getPlatformTarget());
        cache["lastBuild"] = getCurrentTimestamp();
        cache["targets"] = nlohmann::json::object();
        cache["sources"] = nlohmann::json::object();
        cache["steps"] = nlohmann::json::object();

        std::unordered_set<std::string> cachedStep;
        std::unordered_set<std::string> cachedSource;

        for(const auto& targetPair : _buildTargets) {
            auto targetName = targetPair.first;
            auto target = targetPair.second;
            auto& targetJSON = cache["targets"][targetName] = nlohmann::json::object();
            targetJSON["type"] = target->getType();
            targetJSON["dependencies"] = nlohmann::json::array();
            for(const auto& dependency : target->getDependencies()) {
                targetJSON["dependencies"].push_back(dependency);

                if(cachedStep.find(dependency) == cachedStep.end()) {
                    writeBuildStepToCache(dependency,
                        cache,
                        cachedStep,
                        cachedSource);
                    cachedStep.insert(dependency);
                }
            }
            targetJSON["sources"] = nlohmann::json::array();
            for(const auto& source : target->getSources()) {
                targetJSON["sources"].push_back(source);

                if(cachedSource.find(source) == cachedSource.end()) {
                    writeBuildSourceToCache(source,
                        cache);
                    cachedSource.insert(source);
                }
            }
            targetJSON["inputs"] = nlohmann::json::array();
            for(const auto& input : target->getInputs()) {
                targetJSON["inputs"].push_back(input);
            }
            targetJSON["outputs"] = nlohmann::json::array();
            auto outputs = target->getOutputs(this);
            for(const auto& output : outputs) {
                targetJSON["outputs"].push_back(output);
            }
            targetJSON["products"] = nlohmann::json::array();
            for(const auto& product : target->getProducts()) {
                targetJSON["products"].push_back(product);
            }
            targetJSON["lastBuild"] = getCurrentTimestamp();
            targetJSON["config"] = target->getConfigData();
            FF_ASSERT(_targetMetadata.find(targetName) != _targetMetadata.end(), "Target metadata not found for target `%s`.", targetName);
            targetJSON["metadata"] = _targetMetadata[targetName];
        }

        std::filesystem::path cachePath = getObjectDir()/"ff-asset-builder.json";
        std::ofstream file(cachePath);
        file << cache.dump(4); // dump (pretty print) with 4 spaces indent
    }
    void AssetBuilder::writeBuildStepToCache(const std::string& stepName,
        nlohmann::json& cache,
        std::unordered_set<std::string>& cachedSteps,
        std::unordered_set<std::string>& cachedSources) {
        auto step = getBuildStep(stepName);
        auto& stepJSON = cache["steps"][stepName] = nlohmann::json::object();
        stepJSON["type"] = step->getType();
        stepJSON["dependencies"] = nlohmann::json::array();
        for(const auto& dependency : step->getDependencies()) {
            stepJSON["dependencies"].push_back(dependency);

            if(cachedSteps.find(dependency) == cachedSteps.end()) {
                writeBuildStepToCache(dependency,
                    cache,
                    cachedSteps,
                    cachedSources);
                cachedSteps.insert(dependency);
            }
        }
        stepJSON["sources"] = nlohmann::json::array();
        for(const auto& source : step->getSources()) {
            stepJSON["sources"][source] = nlohmann::json::object();

            if(cachedSources.find(source) == cachedSources.end()) {
                writeBuildSourceToCache(source,
                    cache);
                cachedSources.insert(source);
            }
        }
        stepJSON["inputs"] = nlohmann::json::array();
        for(const auto& input : step->getInputs()) {
            stepJSON["inputs"].push_back(input);
        }
        stepJSON["lastBuild"] = getCurrentTimestamp();
        stepJSON["config"] = step->getConfigData();
    }
    void AssetBuilder::writeBuildSourceToCache(const std::string& sourceName,
        nlohmann::json& cache) {
        auto source = getBuildSource(sourceName);
        auto& sourceJSON = cache["sources"][sourceName] = nlohmann::json::object();
        sourceJSON["type"] = source->getType();
        sourceJSON["inputs"] = nlohmann::json::array();
        for(const auto& input : source->getInputs()) {
            sourceJSON["inputs"].push_back(input);
        }
        sourceJSON["lastBuild"] = getCurrentTimestamp();
        sourceJSON["config"] = source->getConfigData();
    }

    void AssetBuilder::writeResourceConfigToConfigJSON(nlohmann::json& resourceCacheObject,
            const pugi::xml_node& resourceConfigNode) {
        for(const auto& attribute : resourceConfigNode.attributes()) {
            resourceCacheObject[attribute.name()] = attribute.value();
        }
        resourceCacheObject["children"] = nlohmann::json::array();
        for(const auto& child : resourceConfigNode.children()) {
            if(child.type() == pugi::node_element) {
                auto childConfigObject = nlohmann::json::object();
                writeResourceConfigToConfigJSON(childConfigObject, child);
                resourceCacheObject["children"].push_back(childConfigObject);
            } else {
                FF_CONSOLE_FATAL("Unsupported condition.");
            }
        }
    }

    std::vector<std::filesystem::path> AssetBuilder::getInputsForBuildTarget(const std::string& targetName) {
        auto target = getBuildTarget(targetName);
        std::vector<std::filesystem::path> inputs;
        inputs.insert(inputs.end(), target->getInputs().begin(), target->getInputs().end());
        for(const auto& dependency : target->getDependencies()) {
            auto dependencyInputs = getInputsForBuildStep(dependency);
            for(const auto& input : dependencyInputs) {
                inputs.push_back(input);
            }
        }
        for(const auto& source : target->getSources()) {
            auto sourceInputs = getInputsForBuildSource(source);
            for(const auto& input : sourceInputs) {
                inputs.push_back(input);
            }
        }
        return inputs;
    }
    std::vector<std::filesystem::path> AssetBuilder::getInputsForBuildStep(const std::string& stepName) {
        auto step = getBuildStep(stepName);
        FF_ASSERT(step != nullptr, "Build step `%s` could not be found.", stepName);
        std::vector<std::filesystem::path> inputs;
        inputs.insert(inputs.end(), step->getInputs().begin(), step->getInputs().end());
        for(const auto& dependency : step->getDependencies()) {
            auto dependencyInputs = getInputsForBuildStep(dependency);
            for(const auto& input : dependencyInputs) {
                inputs.push_back(input);
            }
        }
        for(const auto& source : step->getSources()) {
            auto sourceInputs = getInputsForBuildSource(source);
            for(const auto& input : sourceInputs) {
                inputs.push_back(input);
            }
        }
        return inputs;
    }
    std::vector<std::filesystem::path> AssetBuilder::getInputsForBuildSource(const std::string& sourceName) {
        auto source = getBuildSource(sourceName);
        std::vector<std::filesystem::path> inputs;
        for(const auto& input : source->getInputs()) {
            inputs.push_back(input);
        }
        return inputs;
    }

    bool AssetBuilder::anyConfigChangedForBuildTarget(const std::string& targetName,
        const nlohmann::json& cache) {
        if(cache["targets"].find(targetName) == cache["targets"].end()) {
            return true;
        }

        auto target = getBuildTarget(targetName);
        auto configData = target->getConfigData();
        if(configData != cache["targets"][targetName]["config"]) {
            return true;
        }
        for(const auto& dependency : target->getDependencies()) {
            if(anyConfigChangedForBuildStep(dependency, cache)) {
                return true;
            }
        }
        for(const auto& source : target->getSources()) {
            if(anyConfigChangedForBuildSource(source, cache)) {
                return true;
            }
        }
        return false;
    }
    bool AssetBuilder::anyConfigChangedForBuildStep(const std::string& targetName,
        const nlohmann::json& cache) {
        if(cache["steps"].find(targetName) == cache["steps"].end()) {
            return true;
        }

        auto step = getBuildStep(targetName);
        auto configData = step->getConfigData();
        if(configData != cache["steps"][targetName]["config"]) {
            return true;
        }
        for(const auto& dependency : step->getDependencies()) {
            if(anyConfigChangedForBuildStep(dependency, cache)) {
                return true;
            }
        }
        for(const auto& source : step->getSources()) {
            if(anyConfigChangedForBuildSource(source, cache)) {
                return true;
            }
        }
        return false;
    }
    bool AssetBuilder::anyConfigChangedForBuildSource(const std::string& targetName,
        const nlohmann::json& cache) {
        if(cache["sources"].find(targetName) == cache["sources"].end()) {
            return true;
        }

        auto source = getBuildSource(targetName);
        auto configData = source->getConfigData();
        return configData != cache["sources"][targetName]["config"];
    }
}

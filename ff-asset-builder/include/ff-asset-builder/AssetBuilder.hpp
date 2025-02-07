/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_ASSET_BUILDER_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_ASSET_BUILDER_HPP

#include <ff-asset-builder/BuildTarget.hpp>
#include <ff-asset-builder/BuildStep.hpp>
#include <ff-asset-builder/BuildSource.hpp>

#include <ff/Console.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>

namespace ff {
    enum class PlatformTarget {
        WINDOWS,
        MACOS,
        ANDROID,
        IOS,
        IOS_SIMULATOR,
        UNKNOWN
    };
    std::string convertPlatformTargetToString(const PlatformTarget& platform);
    PlatformTarget convertStringToPlatformTarget(const std::string& platform);
    enum class GraphicsTarget {
        METAL,
        GL,
        GLES,
        UNKNOWN
    };
    std::string convertGraphicsTargetToString(const GraphicsTarget& graphics);
    GraphicsTarget convertStringToGraphicsTarget(const std::string& graphics);

    class AssetBuilder final {
    public:
        AssetBuilder(const std::filesystem::path& sourceDir,
            const std::filesystem::path& targetDir,
            const PlatformTarget& platform,
            const std::vector<GraphicsTarget>& graphics);
        ~AssetBuilder();

        static constexpr int BUNDLE_VERSION = 0;

        const std::filesystem::path& getSourceDir() const;
        const std::filesystem::path& getObjectDir() const;
        const std::filesystem::path& getTargetDir() const;

        const PlatformTarget& getPlatformTarget() const;
        const std::vector<GraphicsTarget>& getGraphicsTargets() const;

        bool isProductionBuild() const;
        bool isDevelopmentBuild() const;

        std::shared_ptr<BuildTarget> addBuildTarget(const std::string& name,
            const std::shared_ptr<BuildTarget>& target);
        std::shared_ptr<BuildTarget> getBuildTarget(const std::string& name) const;
        std::shared_ptr<BuildStep> addBuildStep(const std::string& name,
            const std::shared_ptr<BuildStep>& step);
        std::shared_ptr<BuildSource> addBuildSource(const std::string& name,
            const std::shared_ptr<BuildSource>& source);
        std::shared_ptr<BuildStep> buildAndGetBuildStep(const std::string& name);
        std::shared_ptr<BuildStep> getBuildStep(const std::string& name) const;
        const std::unordered_map<std::string, std::shared_ptr<BuildSource>> getBuildSources();
        std::shared_ptr<BuildSource> getBuildSource(const std::string& name) const;

        bool runCommand(const std::string& command);

        void clean();

        void prepare();
        void addTargetToBuild(const std::string& name);
        void addTargetsForDeltaBuild();
        void addAllTargetsToBuild();
        void build();
        void buildAll();

        void invalidateForDistribution();

    private:
        std::filesystem::path _sourceDir;
        std::filesystem::path _objDir;
        std::filesystem::path _targetDir;

        PlatformTarget _platformTarget;
        std::vector<GraphicsTarget> _graphicsTargets;

        std::unordered_map<std::string, std::shared_ptr<BuildTarget>> _buildTargets;
        std::unordered_map<std::string, std::shared_ptr<BuildStep>> _buildSteps;
        std::unordered_map<std::string, std::shared_ptr<BuildSource>> _buildSources;

        std::vector<std::string> _targetsToBuild;

        std::unordered_map<std::string, nlohmann::json> _targetMetadata;

        bool _isValidForDistribution;

        void validateInputs();
        pugi::xml_document loadDirectory();
        void parseDirectory();
        void storeTargetMetadataFromCache(const std::string& targetName,
            const nlohmann::json& cache);
        void buildTargets();
        void writeIndex();
        void cleanTargetDir();
        void writeBuilderCache();
        void writeBuildStepToCache(const std::string& stepName,
            nlohmann::json& cache,
            std::unordered_set<std::string>& cachedSteps,
            std::unordered_set<std::string>& cachedSources);
        void writeBuildSourceToCache(const std::string& sourceName,
            nlohmann::json& cache);
            
        void writeResourceConfigToConfigJSON(nlohmann::json& resourceCacheObject,
            const pugi::xml_node& resourceConfigNode);

        std::vector<std::filesystem::path> getInputsForBuildTarget(const std::string& targetName);
        std::vector<std::filesystem::path> getInputsForBuildStep(const std::string& stepName);
        std::vector<std::filesystem::path> getInputsForBuildSource(const std::string& sourceName);

        bool anyConfigChangedForBuildTarget(const std::string& targetName,
            const nlohmann::json& cache);
        bool anyConfigChangedForBuildStep(const std::string& targetName,
            const nlohmann::json& cache);
        bool anyConfigChangedForBuildSource(const std::string& targetName,
            const nlohmann::json& cache);
    };
}

#endif

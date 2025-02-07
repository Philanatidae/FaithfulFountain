/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdlib.h>

#include <ff/Console.hpp>
#include <ff/CommandLineOptions.hpp>
#include <ff/CVars.hpp>

#include <ff-asset-builder/AssetBuilder.hpp>

#include <ff/GameServicer.hpp>
#include <ff-support-desktop/DesktopEnvironment.hpp>

#include <ff/util/String.hpp>

int main(int argc, char** argv) {
    using namespace ff;

    FF_CONSOLE_LOG("Faithful Fountain Asset Builder");

    CommandLineOptions commandLineOptions(argc, argv, 1);

    ff::GameServicer gameServicer(std::unique_ptr<ff::GameLoop>(new NullGameLoop()));
    gameServicer.preInit(commandLineOptions, new DesktopEnvironment());

    const std::string inputDirectory = CVars::get<std::string>("asset_builder_input_directory");
    const std::string outputDirectory = CVars::get<std::string>("asset_builder_output_directory");
    const std::string platformName = CVars::get<std::string>("asset_builder_platform_name");
    const std::string graphicsBackend = CVars::get<std::string>("asset_builder_graphics_backend");

    using namespace ff;

    std::vector<GraphicsTarget> graphicsTargets;
    std::vector<std::string> graphicsBackends = split(graphicsBackend, ',');
    for (const std::string& graphicsBackend : graphicsBackends) {
        graphicsTargets.push_back(convertStringToGraphicsTarget(graphicsBackend));
    }

    AssetBuilder assetBuilder(inputDirectory,
        outputDirectory,
        convertStringToPlatformTarget(platformName),
        graphicsTargets);

    if(commandLineOptions.getCommandList().size() == 1
        && toLower(commandLineOptions.getCommandList()[0]) == "clean") {
        FF_CONSOLE_LOG("Task to perform: clean.");
        FF_CONSOLE_LOG("Cleaning...");
        assetBuilder.clean();
    } else {
        FF_CONSOLE_LOG("Task to perform: build.");
        FF_CONSOLE_LOG("Preparing...");
        assetBuilder.prepare();
        FF_CONSOLE_LOG("Adding targets to build...");
        assetBuilder.addTargetsForDeltaBuild();
        FF_CONSOLE_LOG("Begin build...");
        assetBuilder.build();
    }

    FF_CONSOLE_LOG("Done.");

    return EXIT_SUCCESS;
}

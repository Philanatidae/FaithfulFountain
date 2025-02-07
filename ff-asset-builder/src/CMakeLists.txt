# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

target_sources(ff-asset-builder PRIVATE
    AssetBuilder.cpp
    AudioBuildTarget.cpp
    BuildSource.cpp
    BuildStep.cpp
    BuildTarget.cpp
    CVarDefaults.cpp
    entry.cpp
    GLShaderFunctionBuildStep.cpp
    GLShaderLibraryBuildTarget.cpp
    main.cpp
    MaterialBuildTarget.cpp
    MetalShaderFunctionBuildStep.cpp
    MetalShaderLibraryBuildTarget.cpp
    ModelBuildTarget.cpp
    MSDFBitmapFontBuildTarget.cpp
    RawBuildTarget.cpp
    SimpleTextureBuildTarget.cpp
    TextureAtlasBuildTarget.cpp
    TextureBuildSource.cpp
    TextureRegionBuildTarget.cpp
)

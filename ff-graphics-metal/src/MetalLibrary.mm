/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalLibrary.hmm>

#include <ff/Console.hpp>
#include <ff/Locator.hpp>
#include <ff-graphics-metal/MetalGraphicsDevice.hmm>

#include <ff/util/Macros.hpp>

#include <ff/io/BinaryMemory.hpp>

namespace ff {
    MetalLibrary::MetalLibrary(id<MTLLibrary> library)
        :_library(library) {
    }
    MetalLibrary::MetalLibrary(ff::IAssetBundle& assetBundle, const nlohmann::json& assetObject) {
        FF_ASSET_TYPE_CHECK(assetObject, "MetalShaderLibrary");

        FF_ASSERT(!assetObject["path"].is_null(), "Missing key `path` in library `%s`.", assetObject["name"]);
        FF_ASSERT(!assetObject["format"].is_null(), "Missing key `format` in library `%s`.", assetObject["name"]);
        
        auto readerPtr = assetBundle.getAssetReader(assetObject["path"]);
        
        ff::MetalGraphicsDevice& graphics = static_cast<ff::MetalGraphicsDevice&>(ff::Locator::getGraphicsDevice());
        NSError* compileError;
        
        auto memory = std::make_shared<BinaryMemory>(*readerPtr);

        if(assetObject["format"] == "binary") {
            dispatch_data_t libraryData = dispatch_data_create(memory->data(), memory->size(), nullptr, nullptr);
            _library = [graphics.getDevice() newLibraryWithData:libraryData error:&compileError];
        }
        if(assetObject["format"] == "text") {
            NSString* src = [NSString stringWithUTF8String:std::string(reinterpret_cast<char const*>(memory->data()), memory->size() * sizeof(char)).c_str()];

            MTLCompileOptions* compileOptions = [MTLCompileOptions new];
            compileOptions.languageVersion = MTLLanguageVersion2_2;
            _library = [graphics.getDevice() newLibraryWithSource:src
                options:compileOptions
                error:&compileError];
        }
        
        if(!_library) {
            FF_CONSOLE_ERROR("Could not create Metal library: %s", [[compileError localizedDescription] UTF8String]);
        }
    }
    MetalLibrary::~MetalLibrary() {
    }

    id<MTLFunction> MetalLibrary::getFunction(const std::string& name) {
        if(_cachedFns.find(name) == _cachedFns.end()) {
            id<MTLFunction> fn = [_library newFunctionWithName: [NSString stringWithUTF8String:name.c_str()]];
            if(!fn) {
                FF_CONSOLE_ERROR("Metal function `%s` could not be found in the library.", name);
                return nil;
            }
            _cachedFns.insert(std::make_pair(name, fn));
        }
        return _cachedFns[name];
    }
}

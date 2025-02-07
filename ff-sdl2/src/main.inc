/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// The purpose of this file is to contain both the code for macOS and Windows/Linux
// using #if statements. This file contains Objective-C++ code for macOS, but it is
// disabled for the platforms that use C++ instead.
//
// This file is named `main.inc` instead of `main.hpp` so that clangd doesn't throw
// me errors all the time on macOS (the __APPLE__ defines exist, so there is
// Objective-C++ code in the `main.cpp` file).

#ifndef _FF_SDL2_MAIN_HPP
#define _FF_SDL2_MAIN_HPP

#include <ff/Console.hpp>
#include <ff/GameServicer.hpp>
#include <ff/CVars.hpp>

#include <stdio.h>

#include <ff/Locator.hpp>
#include <ff/BackendProvider.inc>

#include <ff/assets/DirectoryAssetBundle.hpp>

#if defined(FF_HAS_SUPPORT_APPLE)
#include <ff-support-apple/Helpers.hpp>
#endif
#if defined(FF_APPLE_USE_NSBUNDLE)
#include <ff-support-apple/NSBundleAssetBundle.hmm>
#endif

#if defined(FF_HAS_GRAPHICS_METAL)
#include <ff-graphics-metal/MetalGraphicsDevice.hmm>
#import <AppKit/AppKit.h>
#endif

#include <SDL.h>
#include <SDL_syswm.h>
#include <ff-sdl2/SDLGameLoop.hpp>
#include <ff-sdl2/SDLEnvironment.hpp>
#include <ff-sdl2/SDLConsole.hpp>

// SDL2main
int main(int argc, char* argv[]) {
    using namespace ff;

    // NOTE: Do NOT use SDL render functions.
    if(SDL_Init(SDL_INIT_VIDEO
        | SDL_INIT_GAMECONTROLLER) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    Console::setConsoleInstance(new SDLConsole());

#if defined(__APPLE__)
    @autoreleasepool {
        #if defined(FF_HAS_SUPPORT_APPLE)
            FF_CONSOLE_LOG("Apple support exists.");
        #else
            FF_CONSOLE_ERROR("Apple support does not exist.");
        #endif

        FF_CONSOLE_LOG("Apple device detected. Checking sandbox status...");
        if(!Helpers::isSandboxed()) {
            FF_CONSOLE_WARN("App sandbox is not enabled. It is highly recommended to sandbox the app, and is required when submitting to the App Store.");
        } else {
            FF_CONSOLE_LOG("App sandbox is enabled.");
        }

        #if !defined(FF_APPLE_USE_NSBUNDLE) && defined(FF_IS_RELEASE)
            FF_CONSOLE_WARN("Release build detected, but NSBundle is not enabled. Is is highly recommended to enable NSBundle before distributing.");
        #endif
#else
    {
#endif

        CommandLineOptions commandLineOptions(argc, argv, 1);
        GameServicer gameServicer(std::make_unique<SDLGameLoop>());
        SDLGameLoop& gameLoop = static_cast<SDLGameLoop&>(gameServicer.getGameLoop());
        gameServicer.preInit(commandLineOptions,
            new SDLEnvironment());

        FF_CONSOLE_LOG("Providing asset bundle...");
        #if defined(FF_APPLE_USE_NSBUNDLE)
            // Might change in the future, but for now use NSBundle
            // if the Apple support library was linked and dev features
            // are turned off. May need to have another flag, FF_RELEASE.
            FF_CONSOLE_LOG("Using NSBundleAssetBundle.");
            Locator::provide(new ff::NSBundleAssetBundle([NSBundle mainBundle], @"Assets"));
        #else
            FF_CONSOLE_LOG("Using DirectoryAssetBundle.");
            Locator::provide(new DirectoryAssetBundle());
        #endif

        FF_CONSOLE_LOG("Providing backends...");
        provideBackends();
        
        gameServicer.init(commandLineOptions);

#if defined(FF_HAS_GRAPHICS_METAL)
        if(Locator::getGraphicsDevice().getBackendName() == GraphicsBackend::METAL) {
            // Game is initialized; macOS uses Metal, so we
            // have to assign the GraphicsDevice's layer to
            // the window's view.
            MetalGraphicsDevice& metalGraphicsDevice = static_cast<MetalGraphicsDevice&>(Locator::getGraphicsDevice());
            SDL_SysWMinfo info;
            SDL_VERSION(&info.version);
            SDL_GetWindowWMInfo(gameLoop.getMainWindow()->getSDLWindowPtr(),
                &info);
            NSWindow* cocoaWin = info.info.cocoa.window;
            cocoaWin.contentView.layer = metalGraphicsDevice.getLayer();
            cocoaWin.contentView.wantsLayer = YES;

            if(CVars::get<bool>("window_macos_disable_fullscreen")
                && CVars::get<bool>("window_resizable")) {
                // Replace primary fullscreen with auxiliary
                [cocoaWin setCollectionBehavior: NSWindowCollectionBehaviorFullScreenAuxiliary
                    | NSWindowCollectionBehaviorManaged];
            }
        }
#endif

        FF_CONSOLE_LOG("Starting game loop...");
        float acculmulator = 0;
        while(gameServicer.getGame().getAlive()) {
            gameServicer.service(acculmulator);
        }

        FF_CONSOLE_LOG("Game loop shutdown.");
    }

    FF_CONSOLE_LOG("Exiting...");

    return EXIT_SUCCESS;
}

#endif


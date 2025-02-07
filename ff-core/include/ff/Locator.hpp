/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_LOCATOR_HPP
#define _FAITHFUL_FOUNTAIN_LOCATOR_HPP

#include <memory>

#include <ff/graphics/IGraphicsDevice.hpp>
#include <ff/assets/IAssetBundle.hpp>
#include <ff/debug/Statistics.hpp>
#include <ff/audio/IAudioBackend.hpp>
#include <ff/env/IEnvironment.hpp>
#include <ff/messages/MessageBus.hpp>
#include <ff/graphics/CameraManager.hpp>

#include <ff/graphics/TextureManagerFwd.hpp>

namespace ff {
    class IGraphicsDevice;
    class IAssetBundle;
    class Statistics;
    class IAudioBackend;
    class IEnvironment;
    class MessageBus;
    class ActorManager;

    class ILocatorImp {
    public:
        ILocatorImp();
        virtual ~ILocatorImp();

        virtual IGraphicsDevice& getGraphicsDevice() = 0;
        virtual void provide(IGraphicsDevice* service) = 0;

        virtual IAssetBundle& getAssetBundle() = 0;
        virtual void provide(IAssetBundle* service) = 0;

        virtual Statistics& getStatistics() = 0;

        virtual IAudioBackend& getAudioBackend() = 0;
        virtual void provide(IAudioBackend* service) = 0;

        virtual IEnvironment& getEnvironment() = 0;
        virtual void provide(IEnvironment* service) = 0;

        virtual MessageBus& getMessageBus() = 0;
        virtual ActorManager& getActorManager() = 0;
        virtual CameraManager& getCameraManager() = 0;
        virtual TextureManager<ColorTexture>& getColorTextureManager() = 0;
        virtual TextureManager<DepthTexture>& getDepthTextureManager() = 0;

        virtual void cleanup() = 0;
    };

    class Locator {
    friend class GameServicer;
    
    public:
        static IGraphicsDevice& getGraphicsDevice();
        static void provide(IGraphicsDevice* graphicsDevice);

        static IAssetBundle& getAssetBundle();
        static void provide(IAssetBundle* assetBundle);

        static Statistics& getStatistics();

        static IAudioBackend& getAudioBackend();
        static void provide(IAudioBackend* audioBackend);

        static IEnvironment& getEnvironment();
        static void provide(IEnvironment* environment);

        static MessageBus& getMessageBus();

        static ActorManager& getActorManager();

        static CameraManager& getCameraManager();
        static TextureManager<ColorTexture>& getColorTextureManager();
        static TextureManager<DepthTexture>& getDepthTextureManager();

    private:
        static std::unique_ptr<ILocatorImp>& getLocatorImpInstance();

        static void cleanup();
    };
}

#include <ff/actors/ActorManager.hpp>
#include <ff/graphics/TextureManager.hpp>

#endif

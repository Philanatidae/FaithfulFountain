/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/Locator.hpp>

#include <memory>

namespace ff {
    ILocatorImp::ILocatorImp() {
    }
    ILocatorImp::~ILocatorImp() {
    }

    class MasterLocatorImp final : public ILocatorImp {
    public:
        MasterLocatorImp() {
            _graphicsDevice = std::make_unique<NullGraphicsDevice>();
            _assetBundle = std::make_unique<NullAssetBundle>();
            _statistics = std::make_unique<Statistics>();
            _audioBackend = std::make_unique<NullAudioBackend>();
            _environment = std::make_unique<NullEnvironment>();
            _messageBus = std::make_unique<MessageBus>();
            _actorManager = std::make_unique<ActorManager>();
            _cameraManager = std::make_unique<CameraManager>();
            _colorTextureManager = std::make_unique<TextureManager<ColorTexture>>();
            _depthTextureManager = std::make_unique<TextureManager<DepthTexture>>();
        }
        ~MasterLocatorImp() {
        }

        IGraphicsDevice& getGraphicsDevice() override {
            return *_graphicsDevice;
        }
        void provide(IGraphicsDevice* service) override {
            _graphicsDevice = std::unique_ptr<IGraphicsDevice>(service);
        }

        IAssetBundle& getAssetBundle() override {
            return *_assetBundle;
        }
        void provide(IAssetBundle* service) override {
            _assetBundle = std::unique_ptr<IAssetBundle>(service);
        }

        Statistics& getStatistics() override {
            return *_statistics;
        }

        IAudioBackend& getAudioBackend() override {
            return *_audioBackend;
        }
        void provide(IAudioBackend* service) override {
            _audioBackend = std::unique_ptr<IAudioBackend>(service);
        }

        IEnvironment& getEnvironment() override {
            return *_environment;
        }
        void provide(IEnvironment* service) override {
            _environment = std::unique_ptr<IEnvironment>(service);
        }

        MessageBus& getMessageBus() override {
            return *_messageBus;
        }

        ActorManager& getActorManager() override {
            return *_actorManager;
        }

        CameraManager& getCameraManager() override {
            return *_cameraManager;
        }

        TextureManager<ColorTexture>& getColorTextureManager() override {
            return *_colorTextureManager;
        }
        TextureManager<DepthTexture>& getDepthTextureManager() override {
            return *_depthTextureManager;
        }

        void cleanup() override {
            _graphicsDevice.reset();
            _assetBundle.reset();
            _statistics.reset();
            _audioBackend.reset();
            _environment.reset();
            _messageBus.reset();
            _actorManager.reset();
            _cameraManager.reset();
            _colorTextureManager.reset();
            _depthTextureManager.reset();
        }

    private:
        std::unique_ptr<IGraphicsDevice> _graphicsDevice;
        std::unique_ptr<IAssetBundle> _assetBundle;
        std::unique_ptr<Statistics> _statistics;
        std::unique_ptr<IAudioBackend> _audioBackend;
        std::unique_ptr<IEnvironment> _environment;
        std::unique_ptr<MessageBus> _messageBus;
        std::unique_ptr<ActorManager> _actorManager;
        std::unique_ptr<CameraManager> _cameraManager;
        std::unique_ptr<TextureManager<ColorTexture>> _colorTextureManager;
        std::unique_ptr<TextureManager<DepthTexture>> _depthTextureManager;
    };

    IGraphicsDevice& Locator::getGraphicsDevice() {
        return getLocatorImpInstance()->getGraphicsDevice();
    }
    void Locator::provide(IGraphicsDevice* graphicsDevice) {
        getLocatorImpInstance()->provide(graphicsDevice);
    }

    IAssetBundle& Locator::getAssetBundle() {
        return getLocatorImpInstance()->getAssetBundle();
    }
    void Locator::provide(IAssetBundle* assetBundle) {
        getLocatorImpInstance()->provide(assetBundle);
    }

    Statistics& Locator::getStatistics() {
        return getLocatorImpInstance()->getStatistics();
    }

    IAudioBackend& Locator::getAudioBackend() {
        return getLocatorImpInstance()->getAudioBackend();
    }
    void Locator::provide(IAudioBackend* audioBackend) {
        getLocatorImpInstance()->provide(audioBackend);
    }

    IEnvironment& Locator::getEnvironment() {
        return getLocatorImpInstance()->getEnvironment();
    }
    void Locator::provide(IEnvironment* environment) {
        getLocatorImpInstance()->provide(environment);
    }

    MessageBus& Locator::getMessageBus() {
        return getLocatorImpInstance()->getMessageBus();
    }

    ActorManager& Locator::getActorManager() {
        return getLocatorImpInstance()->getActorManager();
    }

    CameraManager& Locator::getCameraManager() {
        return getLocatorImpInstance()->getCameraManager();
    }

    TextureManager<ColorTexture>& Locator::getColorTextureManager() {
        return getLocatorImpInstance()->getColorTextureManager();
    }
    TextureManager<DepthTexture>& Locator::getDepthTextureManager() {
        return getLocatorImpInstance()->getDepthTextureManager();
    }

    std::unique_ptr<ILocatorImp>& Locator::getLocatorImpInstance() {
        static std::unique_ptr<ILocatorImp> locatorImp(new MasterLocatorImp());
        return locatorImp;
    }

    void Locator::cleanup() {
        getLocatorImpInstance()->cleanup();
    }
}

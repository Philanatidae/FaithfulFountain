/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

//
//  ViewController.m
//  ff-ios
//
//  Created by Philip Rader on 1/15/21.
//

#import "ViewController.h"

#include <ff/Console.hpp>
#include <ff/GameServicer.hpp>

#include <memory>

#include <ff/BackendProvider.inc>

#include <ff/Locator.hpp>

#include <ff/messages/MessageBus.hpp>
#include <ff/events/input/MouseButtonDownEvent.hpp>
#include <ff/events/input/MouseButtonUpEvent.hpp>
#include <ff/events/input/MouseMoveEvent.hpp>

#include <ff/events/env/EnvPauseRequestedEvent.hpp>

#include <ff/commands/env/EnvPrepareForSuspendCommand.hpp>
#include <ff/commands/env/EnvPrepareForRestoreCommand.hpp>

#include <glm/glm.hpp>

#include <ff-support-apple/NSBundleAssetBundle.hmm>

#import <AVFoundation/AVFoundation.h>

#include <limits>

#include "iOSGameLoop.hmm"
#include "iOSEnvironment.hmm"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    NSLog(@"ViewController::viewDidLoad");
    [super viewDidLoad];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willResignActiveNotification:) name:UIApplicationWillResignActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didBecomeActiveNotification:) name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didEnterBackgroundNotification:) name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willTerminateNotification:) name:UIApplicationWillTerminateNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willEnterForegroundNotification:) name:UIApplicationWillEnterForegroundNotification object:nil];
}

- (void)viewDidLayoutSubviews {
    NSLog(@"ViewController::viewDidlayoutSubviews");
    [super viewDidLayoutSubviews];
    if(UIApplication.sharedApplication.applicationState == UIApplicationStateBackground) {
        // https://stackoverflow.com/a/56957910
        // Prevents viewDidLayoutSubviews from being called more than once when the app goes to the background.
        // iOS does this to take snapshots of the app in portrait and landscape modes for the app switcher.
        // For games (at least so far that I've made) the orientation doesn't matter because we can
        // only be in either portrait or landscape, so no matter the orientation the device is in
        // it will look correct because we will force it to be correct when the user switches back.
        return;
    }
    
    // self.view.insetsLayoutMarginsFromSafeArea = true; According to Apple documentation, this is set to true by default?
    
    // @todo At some point, I would like most of this code to move to `viewDidLoad` instead, since it
    // is more relevant. `viewDidLayoutSubviews` can be used for initialization first time, then
    // subsequent calls (for whatever reason iOS decides) we can just dispatch ResizeEvents.
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(serviceEngine)];
    _gameServicer = new ff::GameServicer(std::make_unique<ff::iOSGameLoop>(self, _displayLink));
    ff::CommandLineOptions commandLineOptions;
    _gameServicer->preInit(commandLineOptions, new ff::iOSEnvironment());
    
    // Provide services before initializing
    FF_CONSOLE_LOG("Providing asset bundle...");
    ff::Locator::provide(new ff::NSBundleAssetBundle([NSBundle mainBundle], @"Assets"));

    FF_CONSOLE_LOG("Providing backends...");
    ff::provideBackends();
    
    _gameServicer->init(commandLineOptions);
    
    ff::MetalGraphicsDevice& graphicsDevice = static_cast<ff::MetalGraphicsDevice&>(ff::Locator::getGraphicsDevice());
    
    // Game is initialized; iOS uses Metal, so we
    // have to add the GraphicDevice's layer to
    // the View.
    _metalLayer = graphicsDevice.getLayer();
    _metalLayer.framebufferOnly = YES; // @todo Move to ff?
    [_metalLayer setFrame:self.view.layer.frame];
    [self.view.layer addSublayer:_metalLayer];
    
    // Bootstrap back buffer dimensions
    CGSize drawableSize = self.view.frame.size;
    CGFloat scaleFactor = [UIScreen mainScreen].scale;
    
    CGFloat topInset = self.view.safeAreaInsets.top;
    CGFloat bottomInset = self.view.safeAreaInsets.bottom;
    
    // Compute safe area padding
    ff::CVars::get<float>("view_safe_area_padding_top") = (float)(topInset / drawableSize.height);
    ff::CVars::get<float>("view_safe_area_padding_bottom") = (float)(bottomInset / drawableSize.height);
    
    FF_CONSOLE_LOG("Starting game loop...");
    _acculmulator = 0;
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}
- (void)viewDidDisappear:(BOOL)animated {
    delete _gameServicer;
}

- (UIRectEdge)preferredScreenEdgesDeferringSystemGesture {
    return UIRectEdgeBottom | UIRectEdgeLeft | UIRectEdgeRight;
}

- (void)getTouchPositionX:(float*)x andY:(float*)y withTouches:(NSSet<UITouch *>*)touches {
    @autoreleasepool {
        NSArray<UITouch*>* touchesArr = [touches allObjects];
        UITouch* touch = touchesArr[0];
        CGPoint touchPoint = [touch locationInView:[self view]];
        CGSize viewSize = self.view.frame.size;
        
        float xAlpha = touchPoint.x / ((float)viewSize.width);
        float yAlpha = (((float)viewSize.height) - touchPoint.y) / ((float)viewSize.height);
        
        *x = glm::mix(-1.0f, 1.0f, xAlpha);
        *y= glm::mix(-1.0f, 1.0f, yAlpha);
    }
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    // @todo Only detect first touch
    float x, y;
    [self getTouchPositionX:&x andY:&y withTouches:touches];

    // Start with a mouse move event to the event position
    ff::Locator::getMessageBus().dispatch<ff::MouseMoveEvent>(glm::vec2(x, y));
    ff::Locator::getMessageBus().dispatch<ff::MouseButtonDownEvent>(ff::MouseButton::PRIMARY, glm::vec2(x, y));
}
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    float x, y;
    [self getTouchPositionX:&x andY:&y withTouches:touches];

    ff::Locator::getMessageBus().dispatch<ff::MouseMoveEvent>(glm::vec2(x, y));
}
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    // @todo Only detect first touch
    float x, y;
    [self getTouchPositionX:&x andY:&y withTouches:touches];

    ff::Locator::getMessageBus().dispatch<ff::MouseButtonUpEvent>(ff::MouseButton::PRIMARY, glm::vec2(x, y));
    // Temporary solution to make touches seem more like a mouse
    // (when touch ends, "mouse" does not move away and buttons look like
    // they're still being hovered)
    ff::Locator::getMessageBus().dispatch<ff::MouseMoveEvent>(glm::vec2(std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max()));
}
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    // @todo Only detect first touch
    float x, y;
    [self getTouchPositionX:&x andY:&y withTouches:touches];

    ff::Locator::getMessageBus().dispatch<ff::MouseButtonUpEvent>(ff::MouseButton::PRIMARY, glm::vec2(x, y));
    // Temporary solution to make touches seem more like a mouse
    // (when touch ends, "mouse" does not move away and buttons look like
    // they're still being hovered)
    ff::Locator::getMessageBus().dispatch<ff::MouseMoveEvent>(glm::vec2(std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max()));
}
 
- (void)willResignActiveNotification:(NSNotification*) notification {
    // This is called when in the "recents" or if the user is
    // moving the app with the home button.
    
    ff::Locator::getMessageBus().dispatch<ff::EnvPauseRequestedEvent>();
}
- (void)didBecomeActiveNotification:(NSNotification*) notification {
    // This is called when the app is back in the foreground after
    // moving or in the "recents"
}
- (void)didEnterBackgroundNotification:(NSNotification*) notification {
    // This is called when the app is in the background (home or
    // switched to another app)
    
    // In ff, this is referred to "environment suspend".
    ff::Locator::getMessageBus().dispatch<ff::EnvPrepareForSuspendCommand>();
    
    // We don't need to remove the _displayLink like I thought, since
    // it is part of CoreAnimation iOS will just not call it.
    //
    // We need to zero out the acculmulator, though, since we don't
    // want the game to catch up  on frames that were in the background.
    _acculmulator = 0;
    if(_gameServicer) {
        static_cast<ff::iOSGameLoop&>(_gameServicer->getGameLoop()).beginForceZeroDeltaTime();
    }
}
- (void)willTerminateNotification:(NSNotification*) notification {
    // This is called when the app is going to terminate (usually
    // by the user removing it from the app switcher, but also
    // iOS may clean us up if it needs extra resources.
    
    ff::Locator::getMessageBus().dispatch<ff::GameShutdownCommand>(); // @todo GameShutdownCommand should have a `force` to quickly cleanup when we aren't allowed time to cleanup like in desktop
    
    FF_CONSOLE_LOG("Deleting game servicer...");
    delete _gameServicer;
    _gameServicer = nullptr;
}
- (void)willEnterForegroundNotification:(NSNotification*) notification {
    // This is called when the app is moving from background
    // to foreground.
    
    // In ff, this is referred to "environment restore".
    ff::Locator::getMessageBus().dispatch<ff::EnvPrepareForRestoreCommand>();
    
    if(_gameServicer) {
        static_cast<ff::iOSGameLoop&>(_gameServicer->getGameLoop()).endForceZeroDeltaTime();
    }
}

- (void)serviceEngine {
    @autoreleasepool {
        _gameServicer->service(_acculmulator);
    }
}


@end

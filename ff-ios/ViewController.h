/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

//
//  ViewController.h
//  ff-ios
//
//  Created by Philip Rader on 1/15/21.
//

#import <UIKit/UIKit.h>
#import <Metal/Metal.h>

#include <ff/GameServicer.hpp>

@interface ViewController : UIViewController

- (void)serviceEngine;

- (void)getTouchPositionX:(float*)x andY:(float*)y withTouches:(NSSet<UITouch *>*)touches;

- (void)willResignActiveNotification:(NSNotification*) notification;
- (void)didBecomeActiveNotification:(NSNotification*) notification;
- (void)didEnterBackgroundNotification:(NSNotification*) notification;
- (void)willTerminateNotification:(NSNotification*) notification;
- (void)willEnterForegroundNotification:(NSNotification*) notification;

@property CAMetalLayer* metalLayer;
@property CADisplayLink* displayLink;
@property float acculmulator;
@property ff::GameServicer* gameServicer;

@end

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-support-apple/Helpers.hpp>

#include <TargetConditionals.h>
#import <Foundation/Foundation.h>

#include <ff/Console.hpp>

#include <nlohmann/json.hpp>
     
#if TARGET_OS_OSX
#import <AppKit/AppKit.h>
#elif TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif

namespace ff {
    bool Helpers::isSandboxed() {
#if TARGET_OS_OSX
        // On macOS, sandboxing is not required. Detect if it is enabled.

        // Source: https://developer.apple.com/forums/thread/45781?answerId=135465022#135465022
        OSStatus err;
        SecCodeRef me;
        CFDictionaryRef dynamicInfo;

        err = SecCodeCopySelf(kSecCSDefaultFlags, &me);
        if(err != errSecSuccess) {
            FF_CONSOLE_ERROR("Error when attempting to detect app sandbox status (macOS). Assuming `false` (not sandboxed).");
            return false;
        }

        err = SecCodeCopySigningInformation(me, (SecCSFlags)kSecCSDynamicInformation, &dynamicInfo);
        if(err != errSecSuccess) {
            FF_CONSOLE_ERROR("Error when attempting to detect app sandbox status (macOS). Assuming `false` (not sandboxed).");
            return false;
        }

        if(!CFDictionaryContainsKey(dynamicInfo, kSecCodeInfoEntitlementsDict)) {
            return false;
        }
        CFDictionaryRef entitlementsDict = (CFDictionaryRef)CFDictionaryGetValue(dynamicInfo, kSecCodeInfoEntitlementsDict);
        if(entitlementsDict == nil) {
            return false;
        }
        if(!CFDictionaryContainsKey(entitlementsDict, CFSTR("com.apple.security.app-sandbox"))) {
            return false;
        }
        NSNumber* appSandboxVal = (NSNumber*)CFDictionaryGetValue(entitlementsDict, CFSTR("com.apple.security.app-sandbox"));
        if(appSandboxVal == nil) {
            return false;
        }
        if([appSandboxVal intValue] == 1) {
            return true;
        }
        return false;
#else
        // iOS, tvOS, etc. require sandboxing.
        // @todo Can we do checks anyway, just to make the code consistent?
        return true;
#endif
    }

    bool Helpers::canOpenURL() {
#if TARGET_OS_OSX
        return true;
#elif TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        return true;
#else
        return false;
#endif
    }

    void Helpers::openURL(const std::string& url) {
        NSURL* address = [NSURL URLWithString:[NSString stringWithFormat:@"https://%@", [NSString stringWithUTF8String:url.c_str()]]];
#if TARGET_OS_OSX
        [[NSWorkspace sharedWorkspace] openURL:address];
#elif TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        [[UIApplication sharedApplication] openURL:address options:@{} completionHandler:nil];
#else
        FF_CONSOLE_ERROR("Opening URLs is not supported on this platform.");
#endif  
    }
}

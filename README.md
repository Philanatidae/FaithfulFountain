# FFRollGame

Current version: v0.1

Remember to update the version in the following places before doing a release:
 - README
 - CVarDefaults (RollGame)
 - macOS plist
 - iOS plist
 - Android version

## Useful bits
[SFINAE](https://eli.thegreenplace.net/2014/sfinae-and-enable_if/)
```
template<typename K, typename std::enable_if<std::is_convertible<K, T>::value, void>::type* = nullptr>
ResourceHandle(const ResourceHandle<K>& other);
```

macOS file command (used to see architectures of executable): `file [filename]`

For template specialization: If getting duplicate symbols, it's because symbols _are_ duplicate, so either 1) the
template has to be `inline`, or 2) the definition has to be in the .cpp file (forward declare the specialization).

App packaging for App Store Connect: https://developer.apple.com/forums/thread/128166
Might be a good script to implement: https://developer.apple.com/forums/thread/73221
Reading on signing: https://www.objc.io/issues/17-security/inside-code-signing/
Signing script? Seems pretty neat: https://gist.github.com/Weptun/5406993

On Android: `app->activity->vm->DetachCurrentThread()` should only be called when the thread is being destroyed:
    - https://groups.google.com/g/android-ndk/c/2H8z5grNqjo
    - https://web.archive.org/web/20111102062643/http://comments.gmane.org/gmane.comp.handhelds.android.ndk/4687

JNI Useful info: https://www.ibm.com/docs/en/sdk-java-technology/8?topic=collector-overview-jni-object-references

## Android `argv`
For Android, `argv` is passed in via activity launch flags. In Android Studio, under the app configuration, under "Launch Flags":
```
--esa clioptions "... args here ..."
```

Strings must be escaped. For example:
```
--esa clioptions "-Dfirst_cvar=false -Dsecond_cvar=10 -Dthird_cvar=\"hello world\""
```

## Android NDK
NDK r23 is required due to API Level 18 (Jelly Bean) support. For OpenGL ES 3.1, minimum API level is 21 (Lollipop). For OpenGL ES 3.2, minimum API level is 24 (Nougat). For Vulkan 1.0, minimum API level is 24 (Nougat). For Vulkan 1.1, minimum API level is API level 28 (Pie). For Vulkan 1.2, minimum API level is API level 33 (Android 13).

Also note the Android version that first supported certain architectures:
 - aarch64 - API Level 21 (Lollipop)
 - x86_64 - API Level 21 (Lollipop)
 - armeabi-v7 - API Level 9 (Gingerbread)
 - x86 - API Level 14 (Ice Cream Sandwich)

## Config Commands

### Desktop
```
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DFF_GAME=RollGame -DFF_DEV_FEATURES=On -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DFF_ENABLE_BACKWARD=Yes ..
```

Release (macOS):
```
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DFF_GAME=RollGame -DFF_DEV_FEATURES=Off -DCMAKE_EXPORT_COMPILE_COMMANDS=Off -DFF_LINK_STATIC=Yes -DFF_LINK_SHARED=No -DFF_APPLE_USE_NSBUNDLE=On ..
```

For macOS Universal 2, add the following: `-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"`

NOTE: Tracy on Windows (0.7.8) uses vcpkg to depend on Freetype2 (2.11.0), which has a problem with MSVC. Tracy profiler (server) cannot be built on Windows until vcpkg is updated with a Freetype2 release (which hasn't happened yet, 9/7/2021).


### iOS
This will need to be streamlined in the future because Faithful Fountain is designed to be loaded in as dynamic libraries. However, iOS (and Android) require static linking of libraries.

Dynamic libraries work in the simulator, however.

Toolchain: [https://github.com/leetal/ios-cmake](https://github.com/leetal/ios-cmake)

1) Building game library:
`SIMULATORARM64` for simulator on Apple Silicon mac.
`OS64` for an actual device.
```
mkdir build-ios && cd build-ios
cmake -GXcode -DFF_DEV_FEATURES=On -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=SIMULATORARM64 -DDEPLOYMENT_TARGET=13.0 -DFF_LINK_SHARED=No -DFF_LINK_STATIC=On ..
```
2) Open the project in Xcode, zero check, build

### Edge Cases
#### iOS
"is not an ios SDK": `sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer`

cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -H. -Bbuild.sim64 -GXcode -DPLATFORM=SIMULATORARM64
cmake --build build.sim64/ --config Release

#### Android
Android Studio comes with an outdated CMake version. To use the CMake on the system, put `cmake.dir` in `local.properties`. Note that it needs to be the path to `bin/cmake`, not `cmake`.

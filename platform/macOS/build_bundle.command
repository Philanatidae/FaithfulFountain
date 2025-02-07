#!/bin/bash
cd "$(dirname "$0")"

echo "Note: remember to build production assets prior to continuing this script. This script does not currently build assets."
read -p "Press enter to continue."

# Clean output directory
echo "Cleaning..."
rm -r build
mkdir build

# Copy template to output directory
echo "Copying template..."
cp -r bundle/"The Brick Game.app" build/"The Brick Game.app"

# Build game
echo "Building release..."
cd ../..
cd build-release
ninja
cd ../platform/macOS

# Copy to folder
echo "Copying executable to bundle..."
cp ../../build-release/ff-desktop/ff-desktop ./build/"The Brick Game.app"/Contents/MacOS/
echo "Allowing binary to be executable..."
chmod +x ./build/"The Brick Game.app"/Contents/MacOS/ff-desktop

# Create Resources directory
echo "Creating Resources directory..."
mkdir ./build/"The Brick Game.app"/Contents/Resources

# Copy icon
# echo "Copying app Icon..."
# cp Icon.icns ./build/"The Brick Game.app"/Contents/Resources/Icon.icns
# TODO: Build XCode assets automatically
cp Icons/build/Assets.car ./build/"The Brick Game.app"/Contents/Resources/Assets.car
cp Icons/build/AppIcon.icns ./build/"The Brick Game.app"/Contents/Appicon.icns

# Build assets
# node ../../Assets/gen/index.js

# Copy assets to Resources folder
echo "Copying pre-built assets to Resources directory..."
cp -r ../../Assets/bin ./build/"The Brick Game.app"/Contents/Resources/Assets
#mv ./build/"The Brick Game.app"/Contents/Resources/bin ./build/"The Brick Game.app"/Contents/Resources/Assets

# Sign executable
echo "Signing executable..."
# codesign -v -f --sign "Apple Development: XXXXX" --entitlements ../../ff-desktop/ff-desktop.entitlements ./build/"The Brick Game.app"/Contents/Appicon.icns
# codesign -v -f --sign "Apple Development: XXXXX" --entitlements ../../ff-desktop/ff-desktop.entitlements ./build/"The Brick Game.app"/Contents/MacOS/ff-desktop
codesign -v -f --sign "Apple Distribution: XXXXX" --deep --entitlements ../../ff-desktop/ff-desktop.entitlements ./build/"The Brick Game.app"

# https://developer.apple.com/forums/thread/128166
productbuild --sign "3rd Party Mac Developer Installer: XXXXX" --component ./build/"The Brick Game.app" /Applications ./build/"The Brick Game.pkg"

echo "Done!"

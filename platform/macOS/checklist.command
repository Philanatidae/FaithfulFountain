#!/bin/bash
cd "$(dirname "$0")"

echo "Use this checklist to confirm that the App bundle was assembled correctly before submitting to Apple."

echo "1) From the output below, verify that the app is executable and has instructions for both x86_64 and arm64:"
find -H ./build/"The Brick Game.app" -print0 | xargs -0 file | grep "Mach-O .*executable"

echo "2) Confirm that the plist is valid"
plutil ./build/"The Brick Game.app"/Contents/Info.plist

# TODO: Confirm assets, icons, etc.

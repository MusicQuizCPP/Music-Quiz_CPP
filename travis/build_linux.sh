#!/usr/bin/env bash
set -e

cmake . -B out
cmake --build out --target format-check-branch

cmake --build out --config Release -j2

if [[ "${SHOULD_PACKAGE}" ]]; then
    ./AppImageBuild/linuxdeploy-x86_64.AppImage --appimage-extract-and-run --appdir AppDir --output appimage --create-desktop-file -e out/bin/MusicQuizGUI -i icons/MusicQuizGUI.png
    ./AppImageBuild/linuxdeploy-plugin-qt-x86_64.AppImage --appdir AppDir
    ./AppImageBuild/linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage
    mv MusicQuizGUI*.AppImage artifacts/MusicQuizGUI.AppImage
fi
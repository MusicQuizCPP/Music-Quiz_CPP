set -e

cmake . -B out

cmake --build out --config Release -j16

if [[ "${SHOULD_PACKAGE}" ]]; then
    export APPIMAGE_EXTRACT_AND_RUN=1
    GSTREAMER_INCLUDE_BAD_PLUGINS=1 ./AppImageBuild/linuxdeploy-x86_64.AppImage  --plugin gstreamer --plugin qt --appdir AppDir --output appimage --create-desktop-file -e out/bin/MusicQuizGUI -i icons/MusicQuizGUI.png  2>&1  > /dev/null
    mv MusicQuizGUI*.AppImage artifacts/MusicQuizGUI.AppImage
fi 
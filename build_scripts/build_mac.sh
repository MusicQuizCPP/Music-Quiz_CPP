#!/usr/bin/env bash
set -e

export Qt5Widgets_DIR=/usr/local/opt/qt/lib/cmake/Qt5Widgets
export Qt5Core_DIR=/usr/local/opt/qt/lib/cmake/Qt5Core
export Qt5_DIR=/usr/local/opt/qt/lib/cmake
export Qt5Gui_DIR=/usr/local/opt/qt/lib/cmake/Qt5Gui
export Qt5Multimedia_DIR=/usr/local/opt/qt/lib/cmake/Qt5Multimedia
export Qt5MultimediaWidgets_DIR=/usr/local/opt/qt/lib/cmake/Qt5MultimediaWidgets
export Qt5OpenGL_DIR=/usr/local/opt/qt/lib/cmake/Qt5OpenGL

cmake . -B out

cmake --build out --config Release -j2
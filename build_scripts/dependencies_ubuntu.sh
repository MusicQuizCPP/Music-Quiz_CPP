#!/usr/bin/env bash
set -e

sudo apt update
sudo apt install -y build-essential git cmake patchelf qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools qtpositioning5-dev libqt5texttospeech5-dev libqt5multimedia5 qtmultimedia5-dev libqt5multimediawidgets5 libqt5multimedia5-plugins qttools5-dev libqt5svg5-dev libqt5opengl5-dev libboost-all-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gir1.2-gst-plugins-bad-1.0 gir1.2-gst-plugins-base-1.0 gir1.2-gstreamer-1.0 gstreamer1.0-tools gstreamer1.0-libav ubuntu-restricted-extras libfuse2
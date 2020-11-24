#!/usr/bin/env bash
set -e

#Install dependencies
curl -LO https://code.qt.io/cgit/qbs/qbs.git/plain/scripts/install-qt.sh;
./install-qt.sh   --version ${QT_VERSION} --toolchain win64_msvc2017_64 qtbase qtsvg qtmultimedia qttools
choco install NuGet.CommandLine
nuget install boost -OutputDirectory /c/boost

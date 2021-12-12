#!/usr/bin/env bash
set -e

#Install dependencies
choco install NuGet.CommandLine -y
choco install 7zip -y
nuget install boost -Version 1.72.0 -OutputDirectory /c/boost

curl -LO https://code.qt.io/cgit/qbs/qbs.git/plain/scripts/install-qt.sh;
./install-qt.sh   --version ${QT_VERSION} --toolchain win64_msvc2017_64 qtbase qtsvg qtmultimedia qttools

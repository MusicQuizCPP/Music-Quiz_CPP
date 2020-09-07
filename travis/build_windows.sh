#!/usr/bin/env bash
set -e

#Install dependencies
curl -LO https://code.qt.io/cgit/qbs/qbs.git/plain/scripts/install-qt.sh;
./install-qt.sh   --version ${QT_VERSION} --toolchain win64_msvc2017_64 qtbase qtsvg qtmultimedia qttools
choco install NuGet.CommandLine
nuget install boost -OutputDirectory /c/boost
choco install nasm
choco install strawberryperl
choco install llvm

#Set path and env variables
export CC=CL.exe 
export CXX=CL.exe 
export "PATH=/c/Qt/${QT_VERSION}/msvc2017_64:/c/Qt/5.12.9/msvc2017_64/bin:$PATH"
export "PATH=/c/Program Files/NASM:$PATH"
export "PATH=/c/Program Files/LLVM/bin:$PATH"
export "PATH=/c/Strawberry/perl/bin:$PATH"
export "BOOST_INCLUDEDIR=/c/boost\boost.1.72.0.0\lib\native\include"
export "BOOST_ROOT=/c/boost\boost.1.72.0.0\lib\native\include"

#Cmake config
cmake . -G "Visual Studio 15 2017 Win64" -B out

#Check formatting
cmake --build out --target format-check-branch

#Compile
cmake --build out --config Release -j2

#package
curl -LO https://github.com/SerGreen/Appacker/releases/download/v1.3.6/Appacker.exe
chmod +x Appacker.exe
mkdir MusicQuizPackage
cp out/bin/Release/MusicQuizGUI.exe MusicQuizPackage/MusicQuizGUI.exe
cp -r installation/dlls/* MusicQuizPackage/
./Appacker.exe -s MusicQuizPackage/ -e MusicQuizGUI.exe -d artifacts/MusicQuizGUI.exe || true

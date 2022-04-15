#!/usr/bin/env bash
set -e

#Set path and env variables
export CC=CL.exe 
export CXX=CL.exe 
export "PATH=/c/Qt/${QT_VERSION}/msvc2017_64:/c/Qt/5.12.9/msvc2017_64/bin:$PATH"
export "BOOST_INCLUDEDIR=/c/boost\boost.1.72.0.0\lib\native\include"
export "BOOST_ROOT=/c/boost\boost.1.72.0.0\lib\native\include"

#Cmake config
cmake . -G "Visual Studio 16 2019" -A x64 -B out

#Compile
cmake --build out --config Release -j2

#package
curl -LO https://github.com/SerGreen/Appacker/releases/download/v1.3.6/Appacker.exe
chmod +x Appacker.exe
mkdir MusicQuizPackage
cp out/bin/Release/MusicQuizGUI.exe MusicQuizPackage/MusicQuizGUI.exe
cp -r installation/dlls/* MusicQuizPackage/
./Appacker.exe -s MusicQuizPackage/ -e MusicQuizGUI.exe -d artifacts/MusicQuizGUI.exe || true
# Installation of Software
  
## Windows

This section will list requirements to compile the application.
The section assumes that you have installed the [choco package manager](https://chocolatey.org/)
and that you have a 2017 version of Visual Studio developer tools with CMake support installed and a bash compatible shell, e.g. [Git Bash](https://gitforwindows.org/)

### Qt 
Run from elevated bash

Download install script
```
curl -LO https://code.qt.io/cgit/qbs/qbs.git/plain/scripts/install-qt.sh
```
Install msvc2017 64bit version
```
./install-qt.sh --version 5.12.9 --toolchain win64_msvc2017_64 qtbase qtsvg qtmultimedia qttools
```

### Download NASM 
Run from elevated bash:

```
choco install nasm
```

### Download Windows compatible PERL 
Run from elevated bash:

```
choco install strawberryperl
```

### Download boost headers
Run from elevated bash

```
choco install NuGet.CommandLine
nuget install boost -OutputDirectory /c/boost
```

### Set Environmental Variables

Windows 10:

- Open Control Panel
- Go to 'System and Security\System'
- Click on 'Advance system settings'
- Click on 'Environmental Variables'
- Under System variables open 'Path'
- Add the following paths:
    ```
    C:/Qt/5.12.9/msvc2017_64
    C:/Program Files/NASM
    C:/Strawberry/perl/bin
    ```
- Add the following other environment variables
  ```
  CC=CL.exe
  CXX=CL.exe
  BOOST_INCLUDEDIR=C:/boost/boost.1.72.0.0/lib/native/include
  BOOST_ROOT=C:/boost/boost.1.72.0.0/lib/native/include
  ```

### Configure Git
Git needs to have disabled lineending conversions:
```
git config --global core.autocrlf input
```

### Compile
check out repository and init submodules
```
git clone https://github.com/MusicQuizCPP/Music-Quiz_CPP.git --recursive
```

Open Visual Studio developer console and go to the cloned folder

Create build directory and run cmake
```
mkdir build
cd build
cmake .. -G "Visual Studio 15 2017 Win64"
```

Go back to repo directory and run compile step. Replace j8 with how many threads you want to compile with
```
cmake --build build --config Release -j8
```


### K-Lite Codec Pack

To use play audio and video on windows the [K-Lite Codec Pack](http://www.codecguide.com/configuration_tips.htm) needs to be installed.


## Linux
This guides assumes an update version of Arch Linux or Ubuntu 20.04
The package names will probably be similair in other distros

### install dependencies

#### Arch

```
pacman -S boost qt5 gst-plugins-bad gst-plugins-bad-libs gst-plugins-base gst-plugins-base-libs gst-plugins-good gst-plugins-ugly cmake nasm perl git gcc make
```

#### Ubuntu 20.04
```
sudo apt install build-essential perl git cmake nasm qt5-default libqt5multimedia5 qtmultimedia5-dev libqt5multimediawidgets5 libqt5multimedia5-plugins qttools5-dev libqt5svg5-dev libqt5opengl5-dev libboost-all-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly
```

### Clone repo and enter it
```
git clone https://github.com/MusicQuizCPP/Music-Quiz_CPP.git --recursive
cd Music-Quiz_CPP
```

### Run Cmake configuration
```
cmake . -B build
```

### Compile
Replace -j8 with how many thread you want to use
```
cmake --build build --config Release -j8
```
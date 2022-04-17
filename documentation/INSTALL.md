# Installation of Software
This guide shows the steps required to build the music quiz.

- [Qt](#qt)
- [Boost](#boost)
- [Git Submodules](#git-submodules)
- [K-Lite Codec Pack](#k-lite-codec-pack)
- [Tips for Installing on Windows 10](#tips-for-installing-on-windows-10)


## Qt
<img src="https://img.shields.io/badge/version-5.14.2-0000FF.svg?style=flat&&labelColor=363D44"/> <img src="https://img.shields.io/badge/bit-64-0000FF.svg?style=flat&&labelColor=363D44"/>

Qt is used for all UI elements.

### Install Qt
Download [Qt](https://www.qt.io/offline-installers) and install it.

> **NOTE** Currently version 5.14.2, 64bit is used.

#### Qt Environmental Variables (Windows)
Add the Qt .dll location to the PATH variable:

```sh
C:\Qt5-14-2\5.14.2\msvc2017
```

```sh
C:\Qt5-14-2\5.14.2\msvc2017\bin
```


## Boost
<img src="https://img.shields.io/badge/version-1.69.0-0000FF.svg?style=flat&&labelColor=363D44"/> <img src="https://img.shields.io/badge/bit-64-0000FF.svg?style=flat&&labelColor=363D44"/>

Download [Boost](https://sourceforge.net/projects/boost/files/boost-binaries/) and install it.

> **NOTE** Currently version 1.69.0, 64bit is used.

### Boost Environmental Variables (Windows)
Add the environmental variable pointing to the root of the directory:

```sh
BOOST_ROOT=C:/local/boost_1_69_0/
```

```sh
BOOST_INCLUDEDIR=C:/local/boost_1_69_0/
```

```sh
BOOST_LIBRARYDIR=C:/local/boost_1_69_0/lib32-msvc-14.1
```

Add location of the binaries to your path.

```sh
C:/local/boost_1_69_0/lib64-msvc-14.1
```


## Git Submodules
The music quiz uses two submodules 'cereal' and 'mdns' to link the quiz up with external ligths that display the different states during a quiz.

### Install Submodules
To install the submodules run:

```sh
git submodule update --init --recursive
```


## K-Lite Codec Pack
To use play audio and video on windows the [K-Lite Codec Pack](http://www.codecguide.com/configuration_tips.htm) needs to be installed.


## Tips for Installing on Windows 10
Below are some guidelines that can help when installing on Windows 10.

- [Command-Line Shell in Windows 10](#command-line-shell-in-windows-10)
- [Setting environmental variables in Windows 10](#setting-environmental-variables-in-windows)
- [Microsoft Visual Studio Configuration File](#microsoft-visual-studio-configuration-file)


### Command-Line Shell in Windows 10
For executing the required commands to install the Inrotech Robot Software it is recommended to use the Windows PowerShell.


### Setting environmental variables in Windows 10
<kbd>Control Panel</kbd> &rarr; <kbd>System and Security</kbd> &rarr; <kbd>System</kbd> &rarr; <kbd>Advanced system settings</kbd> &rarr; <kbd>Environment Variables...</kbd>

or

<kbd>Click start</kbd> &rarr; <kbd>Search for 'variables'</kbd> &rarr; <kbd>Click on 'Edit the system environment variables'</kbd>


### Microsoft Visual Studio Configuration File
The following configuration file (CMakeSettings.json) can be used to compile the code in Visual Studio:

```json
{
    "configurations": [
        {
            "name": "x86-RelWithDebInfo",
            "generator": "Ninja",
            "configurationType": "RelWithDebInfo",
            "inheritEnvironments": [ "msvc_x86" ],
            "buildRoot": "${projectDir}\\out\\build\\${name}",
            "installRoot": "${projectDir}\\out\\install\\${name}",
            "cmakeCommandArgs": "-DCMAKE_PREFIX_PATH=\"C:/Qt/5.12.3/msvc2017/\" -DBUILD_LIGHT_CONTROL=OFF",
            "buildCommandArgs": "",
            "ctestCommandArgs": "",
            "variables": []
        }
    ]
}
```

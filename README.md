[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://travis-ci.com/truschival/DigitalRoosterGui.svg?branch=develop)](https://travis-ci.com/truschival/DigitalRoosterGui)
[![codecov](https://codecov.io/gh/truschival/DigitalRoosterGui/branch/develop/graph/badge.svg)](https://codecov.io/gh/truschival/DigitalRoosterGui)

DigitalRooster
===================

Internet radio and podcast player and alarmclock.

----
# License

Copyright (c) 2018 by Thomas Ruschival <thomas@ruschival.de> 

Licensed under [GNU General Public License v3.0](http://www.gnu.org/licenses/gpl-3.0-standalone.html) 
SPDX-License-Identifier: GPL-3.0-or-later

DigitalRooster uses [Qmlbridgeformaterialfonts](https://github.com/kevincarlson/QmlBridgeForMaterialDesignIcons) 
software by Kevin Carlso licenced under the [SIL Open Font License, Version 1.1.](http://scripts.sil.org/OFL)
QmlBridgeForMaterialDesignIcons uses the True Type Font "materialdesignicons-webfont.ttf"

The font materialdesignicons-webfont.ttf is licensed under [SIL Open
Font License, Version 1.1.](http://scripts.sil.org/OFL) and copyright
belongs to: Copyright (c) 2014, Austin Andrews 

The Name [Material Design Icons](http://materialdesignicons.com/) is a reserved Font Name.  
Copyright (c) 2014, [Google] (http://www.google.com/design/) licensed under
[Apache License Version
2.0](https://github.com/google/material-design-icons/blob/master/LICENSE)

All license details can be found in the file LICENSE	

-----
## Build configuration

### Options & Defaults (compilation flags & targets):

- `-DBUILD_TESTS=On`           build unit tests
- `-DBUILD_GTEST_FROM_SRC=On`  download GoogleTest and build it from source 
                                  (`OFF` requires gtest as external project)
- `-DTEST_COVERAGE=Off`        code coverage
- `-DPROFILING=On`             profiling build for Visual Studio 

Slightly useless configurations

- `-DSETTINGS_FILE_NAME=...` Filename of settings default=`digitalrooster.json`
- `-DSETTINGS_FILE_PATH=...` Where to find configuration file


### Native build Linux or Windows

DigitalRooster requires OpenSSL and QT5.10 to run. For the build a C++14
Compiler, cmake-3.9 are required.

The build was tested with QT5.10 Open Source license with Visual Studio 2017
Community on Windows 7 and Windows 10.

#### Linux prerequisites

QT5.10 is included in Debian Buster or later. Ubuntu should also work.

1. Setup the basic development environment.

    ```
    apt-get install -y \
		bc cmake curl git \
		build-essential g++ gcc \
		doxygen lcov gcovr \
		autoconf automake libtool pkg-config \
		flex bison zip unzip \
		libssl-dev uuid-dev
    ```
	
2. Install QT5 development libraries
	
    ```
	apt-get install -y \
		qt5-default qtbase5-dev-tools \
		qtdeclarative5-dev qtmultimedia5-dev \
		qtquickcontrols2-5-dev qtdeclarative5-dev-tools
    ```

### Docker container for build

If you don't want to install packages on your machine the docker image
`ruschi/devlinuxqtquick2:latest` includes all dependencies to build and run
DigitalRooster.

    docker pull ruschi/devlinuxqtquick2:latest
    docker run -it --privileged --name build_container ruschi/devlinuxqtquick2

Some versions of [docker do not allow the statx system
call](https://github.com/docker/for-linux/issues/208) which is used by the QT
buildtools during MOC generation.  A workaround is to start the docker container
in privileged mode using `--privileged`.


### Build Steps (on Linux)

The following commands will checkout the sources to `/tmp/checkout/`, create a build directory in '/tmp/build/' 
configure and build DigitalRooster.
	
1. Setup directories and checkout
    ```
    export SRC_DIR=/tmp/checkout
    export BUILD_DIR=/tmp/build
    git clone https://github.com/truschival/DigitalRoosterGui.git $SRC_DIR
    ```

2. Configuration 

    ```
    cmake -G "Eclipse CDT4 - Unix Makefiles"  \
    -H$SRC_DIR -B$BUILD_DIR  \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j4 \
    -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=true \
    -DBUILD_TESTS=On \
    -DBUILD_GTEST_FROM_SRC=On \
    -DTEST_COVERAGE=On 
    ```

3. Build

    ```
    cmake --build $BUILD_DIR
    ```

### Optional post build steps

1. Run Tests
 
    The tests must be executed in the build directory.
    ```
    cd $BUILD_DIR
    bin/DigitalRooster_gtest
    ```
    or with lcov coverage output as HTML:
    ```
    cmake --build $BUILD_DIR --target DigitalRooster_gtest_coverage
    ```
	
2. Create Doxygen documentation (if Doxygen is installed)    
    ```
	cmake --build $BUILD_DIR --target DOC
    ```
	
3. Packaging (optional)
    ```
    cd $BUILD_DIR
    cpack
    ```
    
-------

## Runtime configuration

### Podcast, Streams and Alarms

Digitalrooster runs from any directory and generates on the first start, or if
no configuration is found a default configuration.

The configuration path is derived from
[QStandardPaths::ConfigLocation](http://doc.qt.io/qt-5/qstandardpaths.html)
i.e.:
- On Linux :  `~/.config/DigitalRooster/digitalrooster.json` 
- On Windows:  `%LOCALAPPDATA%/DigitalRooster/digitalrooster.json`

### Logging

Digitalrooster supports dynamic logging configuration using
[QLoggingCategory](http://doc.qt.io/qt-5/qloggingcategory.html) i.e.:
- On Linux:   `~/.config/QtProject/qtlogging.ini` 
- On Windows: `%LOCALAPPDATA%/Temp/Digitalrooster.log`

The runtime log file is created in
`QStandardPaths::TempLocation/Digitalrooster.log` i.e.:
- On Linux:   `/tmp/Digitalrooster.log` 
- On Windows: `%LOCALAPPDATA%/Temp/Digitalrooster.log`


#### Example configuration 
All debug messages except for HttpClient and AlarmMonitor are disabled
```
[Rules]
*.debug=false
DigitalRooster.*.debug=true
DigitalRooster.AlarmMonitor.debug=true
DigitalRooster.HttpClient.debug=true
```



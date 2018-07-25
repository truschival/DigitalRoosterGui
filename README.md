[![Build Status](https://travis-ci.com/truschival/DigitalRoosterGui.svg?branch=develop)](https://travis-ci.com/truschival/DigitalRoosterGui)
[![Coverage Status](https://coveralls.io/repos/github/truschival/DigitalRoosterGui/badge.svg?branch=develop)](https://coveralls.io/github/truschival/DigitalRoosterGui?branch=develop)

DigitalRooster
===================

Internet radio and podcast player and alarmclock.

----
# License

Copyright 2018 by Thomas Ruschival <thomas@ruschival.de>

DigitalRooster uses Qmlbridgeformaterialfonts software by Kevin Carlso
(https://github.com/kevincarlson/QmlBridgeForMaterialDesignIcons)
licenced under the SIL Open Font License, Version 1.1.

The font materialdesignicons-webfont.ttf is licensed under SIL Open
Font License, Version 1.1. (http://scripts.sil.org/OFL) and copyright
belongs to: Copyright (c) 2014, Austin Andrews (http://materialdesignicons.com/),
with Reserved Font Name Material Design Icons.  Copyright (c) 2014,
Google (http://www.google.com/design/) uses the license at
(https://github.com/google/material-design-icons/blob/master/LICENSE)

The respective license details can be found in the file LICENSE	

-----
# Build configuration

## Options & Defaults (compilation flags & targets):

- `-DBUILD_TESTS=On`           build unit tests
- `-DBUILD_GTEST_FROM_SRC=On`  download GoogleTest and build it from source (`OFF` requires gtest as external project)
- `-DTEST_COVERAGE=Off`        code coverage
- `-DPROFILING=On`             profiling build for Visual Studio 

Slightly useless configurations

- `-DSETTINGS_FILE_NAME=...` Filename for settings default to digitalrooster.json
- `-DSETTINGS_FILE_PATH=...` Where to find configuration file


## Build pre-requisites

DigitalRooster requires OpenSSL and QT5.10 to run. For the build a C++14
Compiler, cmake-3.0 are required.

### Windows (7/10)

This build was tested with QT5.10 Open Source license with Visual Studio 2017
Community.

### Linux 

QT5.10 is included in Debian Buster or later. Ubuntu should also work.

Setup the basic development environment.

	apt-get update && apt-get install -y \
		bc 	cmake curl 	git \
		build-essential g++ gcc \
		doxygen lcov gcovr \
		autoconf automake libtool pkg-config \
		flex  bison zip unzip \
		libssl-dev 	uuid-dev
	
Install QT5 development libraries
	
	apt-get install -y \
		qt5-default qtbase5-dev-tools \
		qtdeclarative5-dev 	qtmultimedia5-dev \
		qtquickcontrols2-5-dev 	qtdeclarative5-dev-tools


## Build Setup
	
	mkdir build
	
	cd build

### Debug & coverage configuration	

	cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug\
	 -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j4 -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=true \
	 -DBUILD_TESTS=On -DTEST_COVERAGE=On -DBUILD_GTEST_FROM_SRC=On <this_directory>

### Release

	cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release\
	 -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j4 -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=true \
	 <this_directory>

## Build command
	
	cmake --build .  --target all
	
of just 
	
	make -j 9
	
### Create Doxygen documentation (if Doxygen is installed)

	make DOC
	
## Packaging

	cpack

-------







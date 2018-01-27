DigitalRooster
===================

Internet radio and podcast player and alarmclock.

----
#License

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
- -DBUILD_TEST=OFF      (build google test)
                        requires GTest installed as library or `-DBUILD_GTEST_FROM_SRC=On`
- `-DBUILD_GTEST_FROM_SRC=Off` (recompile gtest as external project)
- `-DTEST_COVERAGE=Off`   (code coverage)

Slightly useless configurations
- `-DSETTINGS_FILE_NAME=...` Filename for settings default to digitalrooster.json
- `-DSETTINGS_FILE_PATH=...` Where to find configuration file

## Build Setup
	
	mkdir build
	
	cd build



### Debug & coverage configuration	

	cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug\
	 -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j4 -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=true \
	 -DBUILD_TEST=On -DTEST_COVERAGE=On -DBUILD_GTEST_FROM_SRC=On <this_directory>

### Release

	cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release\
	 -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j4 -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=true \
	 <this_directory>

###Build command
	
	cmake --build .  --target all
	
of just 
	
	make -j 4
	
### Create Doxygen documentation (if Doxygen is installed)

	make DOC
	
-------







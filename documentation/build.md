# Build configuration for GNU/Linux and Windows

DigitalRooster requires OpenSSL >1.0.2 and >QT 5.11 to run.
For the build a C++14 Compiler, cmake-3.10 are required.

DigitalRooster is developped on Debian GNU/Linux using gcc 8.x
with QT 5.12. Any recent Linux distribution should work

On Windows the build was tested with QT5.10 Open Source license
with Visual Studio 2017 Community on Windows 7 and Windows 10.

## Linux prerequisites

QT5.10 is included in Debian Buster or later. Ubuntu should also work.

1.Setup the basic development environment.

    ```sh
    apt-get install -y \
        bc cmake curl git \
        build-essential g++ gcc \
        doxygen lcov gcovr \
        autoconf automake libtool pkg-config \
        flex bison zip unzip \
        libssl-dev uuid-dev
    ```

2.Install QT5 development libraries

    ```sh
    apt-get install -y \
       qt5-default qtbase5-dev-tools \
       qtdeclarative5-dev qtmultimedia5-dev \
       qtquickcontrols2-5-dev qtdeclarative5-dev-tools
    ```

## Docker container for build

If you don't want to install packages on your machine the docker image
`ruschi/devlinuxqtquick2:latest` includes all dependencies to build and run
DigitalRooster.

    ```sh
    docker pull ruschi/devlinuxqtquick2:latest
    docker run -it --privileged --name build_container ruschi/devlinuxqtquick2
    ```

Some versions of [docker do not allow the statx system
call](https://github.com/docker/for-linux/issues/208) which is used by the QT
buildtools during MOC generation.  A workaround is to start the docker container
in privileged mode using `--privileged`.

## Build Steps (on Linux)

### Options & Defaults (compilation flags & targets)

-   `-DBUILD_TESTS=On`           build unit tests

-   `-DBUILD_GTEST_FROM_SRC=On`  download GoogleTest and build it from source
                                  (`OFF` requires gtest as external project)

-   `-DTEST_COVERAGE=Off`        code coverage

-   `-DPROFILING=On`              profiling build for Visual Studio

The following commands will checkout the sources to `/tmp/checkout/`, create a
build directory in `/tmp/build/` configure and build DigitalRooster.

1.Setup directories and checkout

    ```sh
    export SRC_DIR=/tmp/checkout
    export BUILD_DIR=/tmp/build
    git clone https://github.com/truschival/DigitalRoosterGui.git $SRC_DIR
    ```

2.Configuration

    ```sh
    cmake -G "Eclipse CDT4 - Unix Makefiles"  \
    -H$SRC_DIR -B$BUILD_DIR  \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j4 \
    -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=true \
    -DBUILD_TESTS=On \
    -DBUILD_GTEST_FROM_SRC=On \
    -DTEST_COVERAGE=On
    ```

3.Build

    ```sh
    cmake --build $BUILD_DIR
    ```

### Optional post build steps

1.Run Tests
    The tests must be executed in the build directory.

    ```sh
    cd $BUILD_DIR
    bin/DigitalRooster_gtest
    ```

    or with lcov coverage output as HTML:

    ```
    cmake --build $BUILD_DIR --target DigitalRooster_gtest_coverage
    ```

2.Create Doxygen documentation (if Doxygen is installed)

   ```sh
   cmake --build $BUILD_DIR --target DOC
   ```

3.Packaging (optional)

    ```sh
    cd $BUILD_DIR
    cpack
    ```

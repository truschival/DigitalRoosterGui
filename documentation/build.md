# Build configuration for GNU/Linux and Windows

DigitalRooster requires OpenSSL >1.0.2 and >QT 5.11 to run.
For the build a C++14 Compiler, cmake-3.10 are required.

DigitalRooster is developped on Debian GNU/Linux using gcc 8.x
with QT 5.12. Any recent Linux distribution should work

On Windows the build was tested with QT5.10 Open Source license
with Visual Studio 2017 Community on Windows 7 and Windows 10.

## Linux prerequisites

QT5.10 is included in Debian Buster or later. Ubuntu should also work.

(1)  Setup the basic development environment.

``` sh
apt-get install -y \
        bc cmake curl git \
        build-essential g++ gcc \
        doxygen lcov gcovr \
        autoconf automake libtool pkg-config \
        flex bison zip unzip \
        libssl-dev uuid-dev
```

(2) Install QT5 development libraries

``` sh
apt-get install -y \
       qt5-default qtbase5-dev-tools \
       qtdeclarative5-dev qtmultimedia5-dev \
       qtquickcontrols2-5-dev qtdeclarative5-dev-tools
```

(3) Build and install libpistache

If you configure DigitalRoosterGui with  `-DREST_API=On` make sure you have
[pistache](http://pistache.io/) installed.
Unfortunately there are no packages for Debian (yet).
There is a [PPA for ubuntu](https://launchpad.net/~pistache+team/+archive/ubuntu/unstable)

Compiling and installing libpistache is pretty straight-forward.

``` sh
mkdir -p pistache && cd pistache
git clone https://github.com/oktal/pistache.git pistache_src
cd pistache_src
mkdir build && cd build
cmake -DPISTACHE_USE_SSL=On ../
cmake --build . --parallel --target install
sudo ldconfig
```

(4) Python libraries for REST API integration tests

This is only needed if you want to generate and run the python3 integration
tests in [test/api-tests](../test/api-tests)
The following command installs libraries for python 2.x and python 3 on Debian.
``` sh
apt-get install -y \
    python3-pytest python-pytest \
    python3-pytest-cov \
    python3-pytest-runner python-pytest-runner \
    python3-certifi python-certifi\
    python3-urllib3 python-urllib3 \
    python3-setuptools python-setuptools\
    python3-dateutil python-dateutil \
    python3-six python-six \
    python3-requests python-requests \
    python3-git
```
---

## Docker container for build

If you don't want to install packages on your machine the docker image
[ruschi/devlinuxqt-pistache](https://hub.docker.com/r/ruschi/devlinuxqt-pistache)
includes all dependencies to build and run DigitalRooster.

``` sh
docker pull ruschi/devlinuxqt-pistache
docker run -it --privileged --name build_container ruschi/devlinuxqtquick2
```

Some versions of [docker do not allow the statx system
call](https://github.com/docker/for-linux/issues/208) which is used by the QT
buildtools during MOC generation.  A workaround is to start the docker container
in privileged mode using `--privileged`.

---

## Build Steps

All steps to build and run unit tests on your machine in a docker container are
listed the script [buildscripts/build_local.sh](../buildscripts/build_local.sh)

### Options & Defaults (compilation flags & targets)

-   `-DBUILD_TESTS=On`           build unit tests

-   `-DBUILD_GTEST_FROM_SRC=On`  download GoogleTest and build it from source
                                  (`OFF` requires gtest as external project)

-   `-DTEST_COVERAGE=Off`        code coverage

-   `-DREST_API=On`               provide a REST API for json configuration

-   `-DREST_API_PORT=6666`       Default TCP listen port for REST API

-   `-DPROFILING=On`              profiling build for Visual Studio

The following commands will checkout the sources to `/tmp/checkout/`, create a
build directory in `/tmp/build/` configure and build DigitalRooster.

(1) Setup directories and checkout

``` sh
export SRC_DIR=/tmp/checkout
export BUILD_DIR=/tmp/build
git clone https://github.com/truschival/DigitalRoosterGui.git $SRC_DIR
```

(2) Configuration

``` sh
cmake -G "Eclipse CDT4 - Unix Makefiles"  \
    -H$SRC_DIR -B$BUILD_DIR  \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j9 \
    -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=true \
    -DBUILD_TESTS=On \
    -DBUILD_GTEST_FROM_SRC=On \
    -DTEST_COVERAGE=On \
	-DREST_API=On
```

(3) Build

``` sh
cmake --build $BUILD_DIR
```

### Optional post build steps

#### Run Tests

``` sh
cd $BUILD_DIR
ctest -V
```

or with lcov coverage output as HTML:

``` sh
cmake --build $BUILD_DIR --target DigitalRooster_gtest_coverage
```

#### Create Doxygen documentation (if Doxygen is installed)

``` sh
make --build $BUILD_DIR --target DOC
```

#### Packaging (optional)

``` sh
cd $BUILD_DIR
cpack
```

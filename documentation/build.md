# Build configuration for GNU/Linux

DigitalRooster requires OpenSSL >= 1.1.1 (or OpenSSL 3.x) and Qt >= 5.12 to run.
For building, a C++17 compiler and CMake >= 3.16 are required.

DigitalRooster is developed on Debian GNU/Linux (Debian 13 Trixie / testing / sid) using GCC or Clang
with Qt 5.15. Any recent Linux distribution should work.

---

## Linux prerequisites (Debian Trixie / Debian 13)

### (1) Setup the basic development environment

Install the essential build tools, compilers, libraries, and utilities via APT:

```sh
sudo apt-get update && sudo apt-get install -y \
    bc cmake ninja-build curl git \
    build-essential g++ gcc \
    doxygen lcov gcovr \
    autoconf automake libtool pkgconf \
    flex bison zip unzip \
    libssl-dev uuid-dev
```

> **Notes on Debian Trixie:**
> - `pkgconf` replaces the deprecated/transitional `pkg-config` package.
> - `ninja-build` is recommended for fast, parallel CMake builds (using `-G Ninja`).

### (2) Install Qt5 development libraries and runtime modules

Install the Qt5 development packages:

```sh
sudo apt-get install -y \
    qtbase5-dev qtbase5-dev-tools \
    qtdeclarative5-dev qtdeclarative5-dev-tools \
    qtmultimedia5-dev qtquickcontrols2-5-dev
```

To run the DigitalRooster GUI application on your Linux desktop, also install the necessary QML modules and multimedia backend plugins:

```sh
sudo apt-get install -y \
    qml-module-qtquick2 \
    qml-module-qtquick-controls2 \
    qml-module-qtquick-layouts \
    qml-module-qtmultimedia \
    qml-module-qtquick-window2 \
    libqt5multimedia5-plugins
```

> **Notes on Debian Trixie:**
> - The obsolete `qt5-default` metapackage has been removed from Debian since Bullseye and is not available in Trixie. Qt build tools and libraries are configured directly via `qtbase5-dev` and `qtbase5-dev-tools`.

### (3) Install libpistache (for REST API)

If you configure DigitalRooster with `-DREST_API=On`, Pistache is required.

In Debian Trixie (Debian 13), Pistache is available as an official Debian package:

```sh
sudo apt-get install -y libpistache-dev
```

### (4) Python 3 and PyPI packages for REST API integration tests

Running the Python integration tests in [test/api-tests](../test/api-tests) and generating the OpenAPI client requires Python 3. The test runner and dependencies are installed from **PyPI** inside a Python virtual environment (rather than through Debian system packages).

Install the system Python 3 and venv module:

```sh
sudo apt-get install -y python3 python3-venv python3-pip
```

Set up and activate a virtual environment in the repository, and install the required test packages from PyPI using `requirements.txt`:

```sh
# Create virtual environment
python3 -m venv .venv

# Activate virtual environment
source .venv/bin/activate

# Install required Python dependencies from PyPI
pip install -r requirements.txt
```

To generate the Python client module before running the REST API tests, you can use the containerized generator:

```sh
podman run --rm -v $(pwd):/local:z \
    docker.io/openapitools/openapi-generator-cli:v6.5.0 generate \
    -i /local/REST/openapi.yml \
    -g python \
    -c /local/REST/generator-config.json \
    -o /local/python-client
```

Or run the client retrieval script (fetches based on your pushed GitHub commit):

```sh
# Run within active virtual environment:
python3 buildscripts/get_openapi_client.py
```

> **Tip:** When running tests with `ctest`, ensure your virtual environment is active (`source .venv/bin/activate`) so that `pytest` and the client modules from PyPI are found.

---

## Container build (Docker / Podman)

If you prefer not to install dependencies on your host machine, the pre-built container image includes all required tools and libraries:

```sh
podman pull ghcr.io/truschival/devlinuxqt-pistache:v1.2.0
podman run -it --privileged --name build_container ghcr.io/truschival/devlinuxqt-pistache:v1.2.0
```

*(Docker can also be used interchangeably with Podman).*

---

## Build Steps

All steps to build and run unit tests locally in a container are demonstrated in the script [buildscripts/build_local.sh](../buildscripts/build_local.sh).

### CMake Options & Defaults

| Option | Default | Description |
|---|---|---|
| `-DBUILD_TESTS` | `On` | Build unit tests |
| `-DBUILD_GTEST_FROM_SRC` | `On` | Download GoogleTest via FetchContent and build from source |
| `-DREST_API` | `Off` | Enable REST API support (requires libpistache) |
| `-DREST_API_PORT` | `6666` | Default TCP listen port for REST API |
| `-DPROFILE` | `Off` | Build with profiling flags |
| `-DTEST_COVERAGE` | `Off` | Enable code coverage instrumentation |

### Build Walkthrough

**(1) Setup directories and checkout**

```sh
export SRC_DIR=/tmp/checkout
export BUILD_DIR=/tmp/build
git clone https://github.com/truschival/DigitalRoosterGui.git $SRC_DIR
```

**(2) Configuration**

```sh
cmake -S $SRC_DIR -B $BUILD_DIR \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=On \
    -DBUILD_GTEST_FROM_SRC=On \
    -DTEST_COVERAGE=On \
    -DREST_API=On
```

*(You can add `-G Ninja` if you have `ninja-build` installed for faster builds).*

**(3) Build**

```sh
cmake --build $BUILD_DIR --parallel
```

### Optional post-build steps

#### Run Tests

Ensure your virtual environment is activated if running REST API integration tests:

```sh
ctest --test-dir $BUILD_DIR -V
```

or with lcov coverage output as HTML:

```sh
cmake --build $BUILD_DIR --target digitalrooster_gtest_coverage
```

#### Create Doxygen documentation (if Doxygen is installed)

```sh
cmake --build $BUILD_DIR --target apidoc
```

#### Packaging (optional)

```sh
cd $BUILD_DIR
cpack
```

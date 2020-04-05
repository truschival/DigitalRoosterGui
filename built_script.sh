#!/bin/bash

set -x
###
# This is the sequence of commands that run in github action build_and_test.yml
# to check if the commands work in local docker container
###


export BUILD_DIR_HOST=/tmp/build
export BUILD_DIR=/tmp/build_in_container
export SRC_DIR=/tmp/src

# name: Create build dir
mkdir -p $BUILD_DIR_HOST
chmod o+w $BUILD_DIR_HOST
touch $BUILD_DIR_HOST/created
ls -la $BUILD_DIR_HOST

mkdir $BUILD_DIR

git clone /home/ruschi/Coding/DigitalRooster $SRC_DIR
cd $SRC_DIR && git checkout feature/RESTcontrol 

cmake -DCMAKE_BUILD_TYPE=Debug  -DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=On \
      -H$SRC_DIR -B$BUILD_DIR \
      -DBUILD_TESTS=On -DTEST_COVERAGE=On -DBUILD_GTEST_FROM_SRC=On -DREST_API=On

cmake --build $BUILD_DIR --parallel

cmake --build $BUILD_DIR --target test

lcov --directory $BUILD_DIR  --capture --output-file $BUILD_DIR/coverage.info

lcov --remove $BUILD_DIR/coverage.info  --output-file $BUILD_DIR/coverage.info \
     "/usr/*" "*/GTestExternal/*" "*/__/*"

genhtml  $BUILD_DIR/coverage.info --output-directory  $BUILD_DIR/lcov_html

#!/bin/bash

###
# This is the sequence of commands that run in github action build_and_test.yml
# to check if the commands work in locally
#
# NOTE: This file is not actively maintained, could be that it doesn't work
###


log_step () {
    echo -e "\e[1;33;4;44m"
    echo -n "$@"
    echo -e "\e[0m"
}

tempdir=$(mktemp -d)

export BUILD_DIR_HOST=$tempdir/build
export BUILD_DIR=$tempdir/build_in_container
export SRC_DIR=$tempdir/src
# in github actions the workspace is where the sources are located
export GITHUB_WORKSPACE=$SRC_DIR
export BUILD_IMAGE=ruschi/devlinuxqt-pistache
export CONTAINER_NAME=buildc
export TEST_ARTIFACT=test-trace.tgz

# name: Create build dir
mkdir -p $BUILD_DIR_HOST
chmod o+w $BUILD_DIR_HOST

log_step "Cloning to $SRC_DIR"
git clone /home/ruschi/Coding/DigitalRooster $SRC_DIR
cd $SRC_DIR && git checkout feature/RESTcontrol 

log_step "Check & pull docker image"
docker --version
docker pull $BUILD_IMAGE

log_step "Start container $CONTAINER_NAME"
docker run -itd -u $UID:$GID --privileged --name $CONTAINER_NAME  \
        -v$GITHUB_WORKSPACE:$SRC_DIR -v$BUILD_DIR_HOST:$BUILD_DIR \
        $BUILD_IMAGE

log_step "Configure in container" 
docker exec $CONTAINER_NAME cmake \
        -DCMAKE_BUILD_TYPE=Debug  \
        -DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=On \
        -H$SRC_DIR -B$BUILD_DIR \
        -DBUILD_TESTS=On -DTEST_COVERAGE=On -DBUILD_GTEST_FROM_SRC=On \
        -DREST_API=On 

log_step "Build in container"
docker exec $CONTAINER_NAME cmake --build $BUILD_DIR --parallel

log_step "Get openapi client"
docker exec -w $SRC_DIR $CONTAINER_NAME buildscripts/get_openapi_client.py

log_step "Run Tests"
docker exec $CONTAINER_NAME cmake --build $BUILD_DIR --target test

log_step "Package results"
tar -C $BUILD_DIR_HOST -czf $TEST_ARTIFACT \
    test_trace.log \
    test/gtest_results.xml

log_step "Gather coverage stats"
docker exec -w $BUILD_DIR  $CONTAINER_NAME \
       lcov --directory $BUILD_DIR --capture --output-file $BUILD_DIR/coverage.info

log_step "Prune coverage stats"
docker exec -w $BUILD_DIR  $CONTAINER_NAME \
       lcov --remove $BUILD_DIR/coverage.info  \
       --output-file $BUILD_DIR/coverage.info \
       "/usr/*" "*/GTestExternal/*" "*/__/*"

log_step "Cleanup"
docker stop $CONTAINER_NAME
docker rm $CONTAINER_NAME
# rm -rf $tempdir

# genhtml  $BUILD_DIR/coverage.info --output-directory  $BUILD_DIR/lcov_html

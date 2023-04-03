# SPDX-License-Identifier: GPL-3.0-or-later
#
# copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de> Licensed under GNU
# PUBLIC LICENSE Version 3 or later

cmake_minimum_required(VERSION 3.10..3.18)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_WARN_DEPRECATED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add local macros
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules/)

# -------------------------------------------------------------------------------
# Project Name
message(STATUS "Running ${CMAKE_COMMAND} v${CMAKE_VERSION}")
project(
    DigitalRooster
    VERSION 1.3.0
    DESCRIPTION "A digital alarm clock and podcast player"
    LANGUAGES CXX C)

string(TIMESTAMP CMAKE_CONFIGURATION_TIME "%Y-%m-%d %H:%M:%S" UTC)

# Package-Config for components that CMake does not support directly
# include(FindPkgConfig)
if(POLICY CMP0071)
    # CMake >=3.10 (https://cmake.org/cmake/help/git-stage/policy/CMP0071.html)
    cmake_policy(SET CMP0071 NEW)
endif()

# -------------------------------------------------------------------------------
# user Configuration + Defaults
# -------------------------------------------------------------------------------
option(BUILD_TESTS "Build Tests" On)
option(HAS_WPA_SUPPLICANT "Target has wpa_supplicant" Off)
option(REST_API "Use the REST API" Off)
option(PROFILE "Build with Profiling" Off)
option(TEST_COVERAGE "Test Coverage" Off)
option(BUILD_GTEST_FROM_SRC "Rebuild google test as external project" On)
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." Off)

# most likely we have wpa_supplicant on Unix,linux etc.
if(UNIX)
    option(HAS_WPA_SUPPLICANT "Target has wpa_supplicant" On)
else(UNIX)
    option(HAS_WPA_SUPPLICANT "Target has wpa_supplicant" Off)
endif(UNIX)

set(SYSTEM_TARGET_NAME
    "Host"
    CACHE STRING "Target: {Host|Embedded}")
set(SETTINGS_FILE_NAME
    "digitalrooster.json"
    CACHE STRING "application configuration file name")
set(WIFI_INTERFACE
    "wlan0"
    CACHE STRING "WIFI interface name")
set(WPA_CTRL_SOCK
    "/var/run/wpa_supplicant/wlan0"
    CACHE STRING "wpa_supplicant interface contrl socket path")
set(SETTINGS_PATH
    "${CMAKE_CURRENT_BINARY_DIR}/settings"
    CACHE PATH "application configuration path")
set(GENERATED_DIR
    "${CMAKE_BINARY_DIR}/generated"
    CACHE INTERNAL "")
set(REST_API_PORT
    "6666"
    CACHE STRING "Rest API TCP Port")

# -------------------------------------------------------------------------------
# Command line parameters or default
# -------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    # Default to Release-Build
    set(CMAKE_BUILD_TYPE
        Release
        CACHE STRING "Choose the type of build : Debug Release RelWithDebInfo"
              FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# User did not provide install prefix via cmake commandline
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${STAGING_DIR})
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# -------------------------------------------------------------------------------
# External Dependencies
# -------------------------------------------------------------------------------
# Threading library for gtest and others use  ${CMAKE_THREAD_LIBS_INIT} to link
# executable with thread support
find_package(Threads REQUIRED)
find_package(OpenSSL 1.0.2 REQUIRED)
find_package(
    Qt5
    COMPONENTS Core
    REQUIRED)
message(STATUS "Found Qt5 version: ${Qt5Core_VERSION_STRING}")

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

# Get the latest abbreviated commit hash of the working branch
execute_process(
    COMMAND git describe --abbrev=8 --always --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE_REV
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

# -------------------------------------------------------------------------------
# Print Summary
# -------------------------------------------------------------------------------

message(STATUS "------------- Config --------- ")
message(STATUS "Project:\t\t ${CMAKE_PROJECT_NAME} - v${PROJECT_VERSION} ")
message(STATUS "Build-type:\t\t ${CMAKE_BUILD_TYPE} ")
message(STATUS "Build UnitTests:\t ${BUILD_TESTS} ")
message(STATUS "Build GoogleTest:\t ${BUILD_GTEST_FROM_SRC} ")
message(STATUS "Test coverage:\t ${TEST_COVERAGE} ")
message(STATUS "Profiling:\t\t ${PROFILE} ")
message(STATUS "Wpa supplicant:\t ${HAS_WPA_SUPPLICANT} ")
message(STATUS "REST API:\t\t ${REST_API} (${REST_API_PORT})")
message(STATUS "Revision:\t\t ${GIT_DESCRIBE_REV} ")
message(STATUS "System:\t\t ${SYSTEM_TARGET_NAME} ")
message(STATUS "------------------------------ ")

# -------------------------------------------------------------------------------
# Input configuration
# -------------------------------------------------------------------------------

# Where to find headers for this project
set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Cmake generated configuration
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in" "${GENERATED_DIR}/config.h"
    ESCAPE_QUOTES @ONLY NEWLINE_STYLE UNIX)

# Find includes in corresponding build directories
include_directories(${PROJECT_INCLUDE_DIR} ${GENERATED_DIR})

# -------------------------------------------------------------------------------
# Output  configuration
# -------------------------------------------------------------------------------
# Since the "standard" cmake template works with component name set it
# SET(COMPONENT_NAME ${PROJECT_NAME})

# Make debug libs visible per default ->  SET_TARGET_PROPERTIES
set(CMAKE_DEBUG_POSTFIX "d")
# (CMAKE_BINARY_DIR = where cmake was invoked and found project())
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/")
# Folder for libraries (during build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib/")
# Put Archives + Libraries togther
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR
    lib
    CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR
    bin
    CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR
    include
    CACHE PATH "Installation directory for header files")
set(INSTALL_CMAKE_DIR
    ${INSTALL_LIB_DIR}/cmake/
    CACHE PATH "Installation directory for CMake files")
set(INSTALL_DOC_DIR
    "doc/${PROJECT_NAME}"
    CACHE PATH "Installation directory for doxygen docs")

export(PACKAGE ${PROJECT_NAME})

# -------------------------------------------------------------------------------
# Compiler & Linker Flags
# -------------------------------------------------------------------------------

set(CUSTOM_CXX_FLAGS "")
set(CUSTOM_C_FLAGS "")
set(CUSTOM_LINK_FLAGS "")

if(MSVC)
    # Force to always compile with W4
    set(WARNING_FLAGS /W3)
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(WARNING_FLAGS -Wall -Wextra -fmessage-length=0 -Wno-unused-parameter)

    list(APPEND CUSTOM_C_FLAGS -ffunction-sections -fdata-sections)
    list(APPEND CUSTOM_CXX_FLAGS -ffunction-sections -fdata-sections)

    # Link final Executable with Map, clean up unused sections
    list(APPEND CUSTOM_LINK_FLAGS -Wl,--gc-sections -Wl,-Map,Symbols.map)
endif()

list(APPEND CUSTOM_C_FLAGS ${WARNING_FLAGS})
list(APPEND CUSTOM_CXX_FLAGS ${WARNING_FLAGS})

if(TEST_COVERAGE AND NOT MSVC)
    include(CodeCoverage) # does not work for MSVC
    append_coverage_compiler_flags()
endif()

if(PROFILE)
    if(NOT MSVC)
        # manually add -g because it could be a profile-release build
        list(APPEND CUSTOM_C_FLAGS -g -pg)
        list(APPEND CUSTOM_CXX_FLAGS -g -pg)
        list(APPEND CUSTOM_LINK_FLAGS -g -pg -O0)
    else(NOT MSVC)
        list(APPEND CUSTOM_LINK_FLAGS -PROFILE)
    endif(NOT MSVC)
endif(PROFILE)

# -------------------------------------------------------------------------------
# Generate documentation
# -------------------------------------------------------------------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_DOC_DIR_NAME "doc")
    # Generate a doxyfile for complete documentation
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                   ${GENERATED_DIR}/Doxyfile @ONLY)

    # detailed module doumentation is not always generated (no ALL after target)
    add_custom_target(
        apidoc
        ${DOXYGEN_EXECUTABLE} ${GENERATED_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generate documentation"
        VERBATIM)

    # install all files flat into /doc/${PROJECT_NAME}/
    install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_DOC_DIR_NAME}/html/
        COMPONENT APIDOC
        DESTINATION ${INSTALL_DOC_DIR}
        OPTIONAL)
endif(DOXYGEN_FOUND)

# -------------------------------------------------------------------------------
# Sub-dirs to build each with its own CMakelists.txt
# -------------------------------------------------------------------------------

# control interface for wpa_supplicant
add_subdirectory(${PROJECT_SOURCE_DIR}/wpa_ctrl/)
add_subdirectory(${PROJECT_SOURCE_DIR}/libsrc/)

if(REST_API)
    add_subdirectory(${PROJECT_SOURCE_DIR}/REST/)
endif(REST_API)

add_subdirectory(${PROJECT_SOURCE_DIR}/qtgui/)

# -------------------------------------------------------------------------------
# configuration install
# -------------------------------------------------------------------------------

# Tests
if(BUILD_TESTS)
    # enable Cmake's make test
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/test/)
endif(BUILD_TESTS)

# --------------------------------------------------------------------
# Generate CPACK installer
# --------------------------------------------------------------------
# SET(CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "Thomas Ruschival")
set(CPACK_PACKAGE_CONTACT "thomas@ruschival.de")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME}-${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_COMPONENT_DEVELOP_DISPLAY_NAME "Libraries + Headers")
set(CPACK_COMPONENT_APIDOC_DISPLAY_NAME "API Documentation")
set(CPACK_COMPONENT_DEVELOP_DESCRIPTION "Components needed for development")
set(CPACK_COMPONENT_APIDOC_DESCRIPTION "API Documentation")
set(CPACK_NSIS_CONTACT "thomas@ruschival.de")
set(CPACK_NSIS_MODIFY_PATH ON)
include(CPack)
message(STATUS "** end CMakeLists.txt **\n")

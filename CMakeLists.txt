# SPDX-License-Identifier: GPL-3.0-or-later
#
# copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
# Licensed under GNU PUBLIC LICENSE Version 3 or later

cmake_minimum_required(VERSION 3.10)
SET(CMAKE_CXX_STANDARD      17)
SET(CMAKE_VERBOSE_MAKEFILE  OFF)
SET(CMAKE_COLOR_MAKEFILE    ON)
SET(CMAKE_WARN_DEPRECATED   ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add local macros
list(APPEND CMAKE_MODULE_PATH  ${CMAKE_SOURCE_DIR}/CMakeModules/)

#-------------------------------------------------------------------------------
# Project Name
MESSAGE( STATUS "Running ${CMAKE_COMMAND} v${CMAKE_VERSION}" )
PROJECT(DigitalRooster
	VERSION 0.13.0
	DESCRIPTION  "A digital alarm clock and podcast player"
	LANGUAGES CXX C
	)

STRING(TIMESTAMP CMAKE_CONFIGURATION_TIME "%Y-%m-%d %H:%M:%S"  UTC)

# Package-Config for components that CMake does not support directly
# include(FindPkgConfig)
if(POLICY CMP0071)
  #CMake >=3.10 (https://cmake.org/cmake/help/git-stage/policy/CMP0071.html)
  cmake_policy(SET CMP0071 NEW)
endif()

#-------------------------------------------------------------------------------
# user Configuration + Defaults
#-------------------------------------------------------------------------------
option(BUILD_TESTS "Build Tests" On)
option(HAS_WPA_SUPPLICANT "Target has wpa_supplicant" Off)
option(REST_API "Use the REST API" Off)
option(PROFILE "Build with Profiling" Off)
option(TEST_COVERAGE "Test Coverage" Off)
option(BUILD_GTEST_FROM_SRC "Rebuild google test as external project" On)
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." Off)

# most likely we have wpa_supplicant on Unix,linux etc.
IF(UNIX)
  option(HAS_WPA_SUPPLICANT "Target has wpa_supplicant" On)
ELSE(UNIX)
  option(HAS_WPA_SUPPLICANT "Target has wpa_supplicant" Off)
ENDIF(UNIX)

SET(SYSTEM_TARGET_NAME "Host"
  CACHE STRING "Target: {Host|Embedded}")
SET(SETTINGS_FILE_NAME "digitalrooster.json"
  CACHE STRING "application configuration file name")
SET(WIFI_INTERFACE "wlan0"
  CACHE STRING "WIFI interface name")
 SET(WPA_CTRL_SOCK "/var/run/wpa_supplicant/wlan0"
  CACHE STRING "wpa_supplicant interface contrl socket path")
SET(SETTINGS_PATH  "${CMAKE_CURRENT_BINARY_DIR}/settings"
  CACHE PATH "application configuration path")
SET(GENERATED_DIR  "${CMAKE_BINARY_DIR}/generated" CACHE INTERNAL "" )
SET(REST_API_PORT "6666" CACHE STRING "Rest API TCP Port")

#-------------------------------------------------------------------------------
# Command line parameters or default
#-------------------------------------------------------------------------------
IF(NOT CMAKE_BUILD_TYPE)
  #Default to Release-Build
  SET(CMAKE_BUILD_TYPE Release
    CACHE STRING "Choose the type of build : Debug Release RelWithDebInfo"
    FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# User did not provide install prefix via cmake commandline
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  SET(CMAKE_INSTALL_PREFIX ${STAGING_DIR} )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#-------------------------------------------------------------------------------
# External Dependencies
#-------------------------------------------------------------------------------
# Threading library for gtest and others use  ${CMAKE_THREAD_LIBS_INIT} to link
# executable with thread support
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(OpenSSL 1.0.2 REQUIRED)
FIND_PACKAGE(Qt5 COMPONENTS Core REQUIRED)
MESSAGE(STATUS "Found Qt5 version: ${Qt5Core_VERSION_STRING}")

# Instruct CMake to run moc automatically when needed.
SET(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

# Get the latest abbreviated commit hash of the working branch
EXECUTE_PROCESS(
  COMMAND git describe --abbrev=8 --always --dirty
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_DESCRIBE_REV
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
)

#-------------------------------------------------------------------------------
# Print Summary
#-------------------------------------------------------------------------------

MESSAGE(STATUS "------------- Config --------- " )
MESSAGE(STATUS "Project:\t\t ${CMAKE_PROJECT_NAME} - v${PROJECT_VERSION} " )
MESSAGE(STATUS "Build-type:\t\t ${CMAKE_BUILD_TYPE} " )
MESSAGE(STATUS "Build UnitTests:\t ${BUILD_TESTS} ")
MESSAGE(STATUS "Build GoogleTest:\t ${BUILD_GTEST_FROM_SRC} ")
MESSAGE(STATUS "Test coverage:\t ${TEST_COVERAGE} ")
MESSAGE(STATUS "Profiling:\t\t ${PROFILE} ")
MESSAGE(STATUS "Wpa supplicant:\t ${HAS_WPA_SUPPLICANT} ")
MESSAGE(STATUS "REST API:\t\t ${REST_API} (${REST_API_PORT})")
MESSAGE(STATUS "Revision:\t\t ${GIT_DESCRIBE_REV} ")
MESSAGE(STATUS "System:\t\t ${SYSTEM_TARGET_NAME} ")
MESSAGE(STATUS "------------------------------ " )

#-------------------------------------------------------------------------------
# Input configuration
#-------------------------------------------------------------------------------

# Where to find headers for this project
SET(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Cmake generated configuration
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
  "${GENERATED_DIR}/config.h"
  ESCAPE_QUOTES
  @ONLY
  NEWLINE_STYLE UNIX
  )

#Find includes in corresponding build directories
include_directories(${PROJECT_INCLUDE_DIR} ${GENERATED_DIR})

#-------------------------------------------------------------------------------
# Output  configuration
#-------------------------------------------------------------------------------
#Since the "standard" cmake template works with component name set it
#SET(COMPONENT_NAME ${PROJECT_NAME})

# Make debug libs visible per default ->  SET_TARGET_PROPERTIES
SET(CMAKE_DEBUG_POSTFIX "d")
# (CMAKE_BINARY_DIR = where cmake was invoked and found project())
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY	"${PROJECT_BINARY_DIR}/bin/")
# Folder for libraries (during build)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY	"${PROJECT_BINARY_DIR}/lib/")
# Put Archives + Libraries togther
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Offer the user the choice of overriding the installation directories
SET(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
SET(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
SET(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
SET(INSTALL_CMAKE_DIR ${INSTALL_LIB_DIR}/cmake/ CACHE PATH
    "Installation directory for CMake files")
SET(INSTALL_DOC_DIR "doc/${PROJECT_NAME}" CACHE PATH
	"Installation directory for doxygen docs")

export(PACKAGE ${PROJECT_NAME})

#-------------------------------------------------------------------------------
# Compiler & Linker Flags
#-------------------------------------------------------------------------------

SET(CUSTOM_CXX_FLAGS "")
SET(CUSTOM_C_FLAGS "")
SET(CUSTOM_LINK_FLAGS "")

IF(MSVC)
  # Force to always compile with W4
  SET(WARNING_FLAGS   /W3 )
ELSEIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  SET(WARNING_FLAGS   -Wall -Wextra -fmessage-length=0 -Wno-unused-parameter )

  LIST(APPEND CUSTOM_C_FLAGS   -ffunction-sections -fdata-sections )
  LIST(APPEND CUSTOM_CXX_FLAGS -ffunction-sections -fdata-sections )

  # Link final Executable with Map, clean up unused sections
  LIST(APPEND CUSTOM_LINK_FLAGS -Wl,--gc-sections -Wl,-Map,Symbols.map )
ENDIF()

LIST(APPEND CUSTOM_C_FLAGS   ${WARNING_FLAGS})
LIST(APPEND CUSTOM_CXX_FLAGS ${WARNING_FLAGS})

IF(TEST_COVERAGE AND NOT MSVC)
  include(CodeCoverage) # does not work for MSVC
  APPEND_COVERAGE_COMPILER_FLAGS()
ENDIF()

IF(PROFILE)
  IF(NOT MSVC)
	# manually add -g because it could be a profile-release build
	LIST(APPEND CUSTOM_C_FLAGS     -g -pg )
	LIST(APPEND CUSTOM_CXX_FLAGS   -g -pg )
	LIST(APPEND CUSTOM_LINK_FLAGS  -g -pg -O0 )
  ELSE(NOT MSVC)
	LIST(APPEND  CUSTOM_LINK_FLAGS -PROFILE )
  ENDIF(NOT MSVC)
ENDIF(PROFILE)

#-------------------------------------------------------------------------------
# Generate documentation
#-------------------------------------------------------------------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
    SET(DOXYGEN_DOC_DIR_NAME "doc")
    # Generate a doxyfile for complete documentation
    configure_file(
	  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
      ${GENERATED_DIR}/Doxyfile
	  @ONLY
	  )

    # detailed module doumentation is not always generated (no ALL after target)
    add_custom_target(apidoc
        ${DOXYGEN_EXECUTABLE} ${GENERATED_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generate documentation"
		VERBATIM
    )

   # install all files flat into /doc/${PROJECT_NAME}/
   INSTALL(DIRECTORY
       ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_DOC_DIR_NAME}/html/
       COMPONENT APIDOC
       DESTINATION ${INSTALL_DOC_DIR}
	   OPTIONAL
    )
endif(DOXYGEN_FOUND)

#-------------------------------------------------------------------------------
# Sub-dirs to build each with its own CMakelists.txt
#-------------------------------------------------------------------------------

# control interface for wpa_supplicant
add_subdirectory(${PROJECT_SOURCE_DIR}/wpa_ctrl/)
add_subdirectory(${PROJECT_SOURCE_DIR}/libsrc/)

if(REST_API)
  add_subdirectory(${PROJECT_SOURCE_DIR}/REST/)
endif(REST_API)

add_subdirectory(${PROJECT_SOURCE_DIR}/qtgui/)

#-------------------------------------------------------------------------------
# configuration install
#-------------------------------------------------------------------------------

# Tests
if(BUILD_TESTS)
  # enable Cmake's make test
  enable_testing()
  add_subdirectory(${PROJECT_SOURCE_DIR}/test/)
endif(BUILD_TESTS)

# --------------------------------------------------------------------
# Generate CPACK installer
# --------------------------------------------------------------------
#SET(CPACK_GENERATOR TGZ)
SET(CPACK_PACKAGE_NAME ${PROJECT_NAME})
SET(CPACK_PACKAGE_VENDOR  "Thomas Ruschival")
SET(CPACK_PACKAGE_CONTACT "thomas@ruschival.de")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
SET(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME}-${PROJECT_VERSION})
SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
SET(CPACK_COMPONENT_DEVELOP_DISPLAY_NAME "Libraries + Headers")
SET(CPACK_COMPONENT_APIDOC_DISPLAY_NAME "API Documentation")
SET(CPACK_COMPONENT_DEVELOP_DESCRIPTION "Components needed for development")
SET(CPACK_COMPONENT_APIDOC_DESCRIPTION  "API Documentation")
SET(CPACK_NSIS_CONTACT "thomas@ruschival.de")
SET(CPACK_NSIS_MODIFY_PATH ON)
INCLUDE(CPack)
MESSAGE(STATUS "** end CMakeLists.txt **\n")

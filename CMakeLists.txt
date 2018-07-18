##
# Top-level CMakeLists.txt
##

cmake_minimum_required (VERSION 3.2)
SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_VERBOSE_MAKEFILE  OFF)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_WARN_DEPRECATED ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add local macros
list(APPEND CMAKE_MODULE_PATH  ${CMAKE_SOURCE_DIR}/CMakeModules/)

#-------------------------------------------------------------------------------
# Project Name
project (DigitalRooster
	VERSION 0.2.3 
	DESCRIPTION  "A digital alarm clock and podcast player"
	LANGUAGES CXX C)

string(TIMESTAMP CMAKE_CONFIGURATION_TIME "%Y-%m-%d %H:%M:%S"  UTC)
message(STATUS "Running ${CMAKE_COMMAND} ${CMAKE_VERSION} on ${CMAKE_PROJECT_NAME} v${PROJECT_VERSION}" )

# Package-Config for components that CMake does not support directly
# include(FindPkgConfig)

if(POLICY CMP0071)
	# POLICY since CMake 3.10 (https://cmake.org/cmake/help/git-stage/policy/CMP0071.html)
	cmake_policy(SET CMP0071 NEW)
endif()

#-------------------------------------------------------------------------------
# user Configuration + Defaults
#-------------------------------------------------------------------------------
option(BUILD_EXE "Build QT Gui" On)
option(BUILD_TESTS "Build Tests" On)
option(PROFILE "Build with Profiling" OFF)
option(TEST_COVERAGE "Test Coverage" OFF)
option(BUILD_GTEST_FROM_SRC "Rebuild google test as external project" On)
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." Off)

SET(SETTINGS_FILE_NAME "digitalrooster.json" CACHE STRING "application configuration file name")
SET(SETTINGS_PATH  "${CMAKE_CURRENT_BINARY_DIR}/settings" CACHE PATH "application configuration path")
set(GENERATED_DIR  "${CMAKE_BINARY_DIR}/generated" CACHE INTERNAL "" )

#-------------------------------------------------------------------------------
# Command line parameters  or default
#-------------------------------------------------------------------------------
IF(NOT CMAKE_BUILD_TYPE)
    #Default to Release-Build
    SET(CMAKE_BUILD_TYPE Release
		CACHE STRING "Choose the type of build : Debug Release RelWithDebInfo"
		FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# User did not provide install prefix via cmake commandline
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  SET(CMAKE_INSTALL_PREFIX ${STAGING_DIR} )
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#-------------------------------------------------------------------------------
# External Dependencies
#-------------------------------------------------------------------------------
# Threading library for gtest and others use  ${CMAKE_THREAD_LIBS_INIT} to link
# executable with thread support
find_package(Threads REQUIRED)

find_package(OpenSSL 1.0.2 REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Multimedia REQUIRED)
MESSAGE(STATUS "Found Qt5 version: ${Qt5Widgets_VERSION_STRING}")

#-------------------------------------------------------------------------------
# Print Summary
#-------------------------------------------------------------------------------

message(STATUS "------------- Config --------- " )
message(STATUS "Build-type:\t\t ${CMAKE_BUILD_TYPE} " )
message(STATUS "Build UnitTests:\t ${BUILD_TESTS} ")
message(STATUS "Build GoogleTest:\t ${BUILD_GTEST_FROM_SRC} ")
message(STATUS "Test coverage:\t ${TEST_COVERAGE} ")
message(STATUS "Profiling:\t\t ${PROFILE} ")
message(STATUS "------------------------------ " )

#-------------------------------------------------------------------------------
# Input configuration
#-------------------------------------------------------------------------------

# Where to find headers for this project
SET(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Cmake generated configuration
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
  "${GENERATED_DIR}/config.h" @ONLY NEWLINE_STYLE UNIX)

#Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_INCLUDE_DIR} ${GENERATED_DIR})

#-------------------------------------------------------------------------------
# Output  configuration
#-------------------------------------------------------------------------------
#Since the "standard" cmake template works with component name set it
set(COMPONENT_NAME ${PROJECT_NAME})

# Make debug libs visible per default ->  SET_TARGET_PROPERTIES
SET(CMAKE_DEBUG_POSTFIX "_dbg")
SET(CMAKE_TESTCOVERAGE_POSTFIX "_cov")
# (CMAKE_BINARY_DIR = where cmake was invoked and found project())
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY	"${PROJECT_BINARY_DIR}/bin/")
# Folder for libraries (during build)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY	"${PROJECT_BINARY_DIR}/lib/")
# Put Archives + Libraries togther
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH  "Installation directory for header files")
set(INSTALL_CMAKE_DIR ${INSTALL_LIB_DIR}/cmake/ CACHE PATH
    "Installation directory for CMake files")
set(INSTALL_DOC_DIR "doc/${PROJECT_NAME}" CACHE PATH  "Installation directory for doxygen docs")

set(version_config "${GENERATED_DIR}/${COMPONENT_NAME}ConfigVersion.cmake")
set(component_config "${GENERATED_DIR}/${COMPONENT_NAME}Config.cmake")
set(targets_export_name "${COMPONENT_NAME}Targets")
export(PACKAGE ${PROJECT_NAME})

#-------------------------------------------------------------------------------
# Compiler & Linker Flags
# #-------------------------------------------------------------------------------

SET(CUSTOM_CXX_FLAGS "")
SET(CUSTOM_C_FLAGS "")
SET(CUSTOM_LINK_FLAGS "")

IF(MSVC)
  # Force to always compile with W4
  SET(WARNING_FLAGS   /W3 )
ELSEIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  SET(WARNING_FLAGS   -Wall -Wextra -fmessage-length=0 -Wno-unused-parameter )

  LIST(APPEND CUSTOM_C_FLAGS   -ffunction-sections -fdata-sections )
  LIST(APPEND CUSTOM_CXX_FLAGS   -ffunction-sections -fdata-sections )

  # Link final Executable with Map, clean up unused sections
  LIST(APPEND CUSTOM_LINK_FLAGS	 -Wl,--gc-sections -Wl,-Map,Symbols.map )
ENDIF()

LIST(APPEND CUSTOM_C_FLAGS   ${WARNING_FLAGS})
LIST(APPEND CUSTOM_CXX_FLAGS ${WARNING_FLAGS})

IF(TEST_COVERAGE AND NOT MSVC)
	include(CodeCoverage) # does not work for MSVC
	APPEND_COVERAGE_COMPILER_FLAGS()
ENDIF()

IF(PROFILE) 
  IF(NOT MSVC)
	MESSAGE(WARNING "gprof ist not the best...")
	# manually add -g because it could be a profile-release build
	LIST(APPEND CUSTOM_C_FLAGS       -g  -pg )	
	LIST(APPEND CUSTOM_CXX_FLAGS     -g -pg )
	LIST(APPEND CUSTOM_LINK_FLAGS    -O2  -g -pg )
  ELSE(NOT MSVC)
	LIST(APPEND  CUSTOM_LINK_FLAGS     -PROFILE)
  ENDIF(NOT MSVC)
ENDIF(PROFILE)

#-------------------------------------------------------------------------------
# Generate documentation
#-------------------------------------------------------------------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
    SET(DOXYGEN_DOC_DIR_NAME "Doc")
    # Generate a doxyfile for complete documentation
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${GENERATED_DIR}/Doxyfile @ONLY)

    # detailed module doumentation is not always generated (no ALL after target)
    add_custom_target(DOC
        ${DOXYGEN_EXECUTABLE} ${GENERATED_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generate documentation" VERBATIM
    )

   # install all files flat into /doc/${PROJECT_NAME}/
   INSTALL(DIRECTORY
       ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_DOC_DIR_NAME}/html/
       COMPONENT APIDOC
       DESTINATION ${INSTALL_DOC_DIR}  OPTIONAL
    )
endif(DOXYGEN_FOUND)

#-------------------------------------------------------------------------------
# Sub-dirs to build each with its own CMakelists.txt
#-------------------------------------------------------------------------------

# Library source
add_subdirectory(${PROJECT_SOURCE_DIR}/libsrc/)

if(BUILD_EXE)
  add_subdirectory(${PROJECT_SOURCE_DIR}/qtgui/)
endif(BUILD_EXE)

#-------------------------------------------------------------------------------
# configuration install
#-------------------------------------------------------------------------------

#default configuration installation
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/digitalrooster.json" 
           "${SETTINGS_PATH}/${SETTINGS_FILE_NAME}" COPYONLY)

# Tests
if(BUILD_TESTS)
  # enable Cmake's make test
  enable_testing()
  add_subdirectory(${PROJECT_SOURCE_DIR}/test/)
endif(BUILD_TESTS)


# --------------------------------------------------------------------
# Generate CPACK installer
# --------------------------------------------------------------------
#set(CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "Thomas Ruschival")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME}-${PROJECT_VERSION})
SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_COMPONENT_DEVELOP_DISPLAY_NAME "Libraries + Headers")
set(CPACK_COMPONENT_APIDOC_DISPLAY_NAME "API Documentation")
set(CPACK_COMPONENT_DEVELOP_DESCRIPTION "Components needed for development")
set(CPACK_COMPONENT_APIDOC_DESCRIPTION  "API Documentation")

SET(CPACK_NSIS_CONTACT "t.ruschival@gmail.com")
SET(CPACK_NSIS_MODIFY_PATH ON)

INCLUDE(CPack)

MESSAGE(STATUS "** end CMakeLists.txt **\n")

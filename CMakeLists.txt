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
project (DigitalRooster CXX C)
# Some arbitrary hand-maintained version
set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 1)
set(PROJECT_VERSION  ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION})
# Some arbitrary project description - only used in doxygen and cpack
set(PROJECT_DESCRIPTION "A Digital Alarm Clock")
message(STATUS "Running ${CMAKE_COMMAND} ${CMAKE_VERSION} on ${CMAKE_PROJECT_NAME} v${PROJECT_VERSION}" )

# Package-Config for components that CMake does not support directly
# include(FindPkgConfig)

#-------------------------------------------------------------------------------
# user Configuration + Defaults
#-------------------------------------------------------------------------------
option(BUILD_EXE "Build QT Gui" On)
option(BUILD_TEST "Build Tests" On)
option(PROFILE "Build with Profiling" Off)
option(TEST_COVERAGE "Test Coverage" Off)
option(BUILD_GTEST_FROM_SRC "Rebuild google test as external project" Off)
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." Off)

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
# Print Summary
#-------------------------------------------------------------------------------

message(STATUS "------------- Config --------- " )
message(STATUS "Build-type:\t\t ${CMAKE_BUILD_TYPE} " )
message(STATUS "Build UnitTests:\t ${BUILD_TEST} ")
message(STATUS "Install UnitTests:\t ${INSTALL_UNIT_TEST_ON_TARGET} ")
message(STATUS "------------------------------ " )

#-------------------------------------------------------------------------------
# Input configuration
#-------------------------------------------------------------------------------

# Where to find headers for this project
SET(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${PROJECT_INCLUDE_DIR})

#-------------------------------------------------------------------------------
# Output  configuration
#-------------------------------------------------------------------------------
# projectname = name of library (without lib- prefix)
string(TOLOWER "${PROJECT_NAME}" LIBRARY_NAME)
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

set(generated_dir "${CMAKE_BINARY_DIR}/generated")
set(version_config "${generated_dir}/${COMPONENT_NAME}ConfigVersion.cmake")
set(component_config "${generated_dir}/${COMPONENT_NAME}Config.cmake")
set(targets_export_name "${COMPONENT_NAME}Targets")
export(PACKAGE ${PROJECT_NAME})

#-------------------------------------------------------------------------------
# Compiler & Linker Flags
# #-------------------------------------------------------------------------------
SET(GCC_WARNING_FLAGS   " -Wall -Wextra -fmessage-length=0 -Wno-unused-parameter " )

SET(CMAKE_C_FLAGS       " ${CMAKE_C_FLAGS} ${GCC_WARNING_FLAGS} ")
SET(CMAKE_CXX_FLAGS     " --std=c++11 ${CMAKE_CXX_FLAGS} ${GCC_WARNING_FLAGS} ")

SET(CMAKE_CXX_FLAGS_RELEASE  " ${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections  --std=c++11")
SET(CMAKE_C_FLAGS_RELEASE  " ${CMAKE_C_FLAGS_RELEASE} -ffunction-sections -fdata-sections ")

# Link final Executable with Map, clean up unused sections
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE
    " ${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections -Wl,-Map,Symbols.map ")

if(PROFILE) # manually add -g because it could be a profile-release build
    SET(CMAKE_C_FLAGS      " ${CMAKE_C_FLAGS} -g  -pg ")
    SET(CMAKE_CXX_FLAGS    " ${CMAKE_CXX_FLAGS} -g -pg ")
    SET(CMAKE_LINK_FLAGS   " ${CMAKE_LINK_FLAGS} -O2 -g -pg ")
endif()

#-------------------------------------------------------------------------------
# Generate documentation
#-------------------------------------------------------------------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
    SET(DOXYGEN_DOC_DIR_NAME "Doc")
    # Generate a doxyfile for complete documentation
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${generated_dir}/Doxyfile @ONLY)

    # detailed module doumentation is not always generated (no ALL after target)
    add_custom_target(DOC
        ${DOXYGEN_EXECUTABLE} ${generated_dir}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generate documentation" VERBATIM
    )

   # install all files flat into /doc/${PROJECT_NAME}/
   INSTALL(DIRECTORY
       ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_DOC_DIR_NAME}/html/
       COMPONENT APIDOC
       DESTINATION ${INSTALL_DOC_DIR}
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


# Tests
if(BUILD_TEST)
  # enable Cmake's make test
  enable_testing()
  add_subdirectory(${PROJECT_SOURCE_DIR}/test/)
endif(BUILD_TEST)


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

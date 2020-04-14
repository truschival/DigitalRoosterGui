MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

# name of library (without lib- prefix)
set(LIBRARY_NAME restconfig)
# Component name (what to build and install) used in add_executable or add_library
set(COMPONENT_NAME ${LIBRARY_NAME})
# Interface/binary version
SET(COMPONENT_VERSION ${PROJECT_VERSION})
#includes etc. in folder (no subfolder here)
SET(COMPONENT_PATH "")

# Check if Pistache is installed
# Pistache does not yet provide a CMake Findpackage  --> fall back to pkgconfig
include(FindPkgConfig)
pkg_search_module(PISTACHE REQUIRED libpistache>=0.0.2)

# QT5 Components used in library
find_package(Qt5 COMPONENTS Core  Network REQUIRED)

# Threading library for gtest
# Use ${CMAKE_THREAD_LIBS_INIT} for the library
find_package(Threads REQUIRED)

#------------------------------
# add compile definitions
#------------------------------
SET(CPP_DEFS "")
LIST(APPEND CPP_DEFS "")

#-------------------------------------------------------------------------------
# Input sources
#-------------------------------------------------------------------------------
SET(SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/ApiHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/AlarmApi.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PodcastApi.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/RadioApi.cpp
  )

#------------------------------
# Output a library
#------------------------------
ADD_LIBRARY(${COMPONENT_NAME} SHARED
  ${SRCS}
  )

SET_TARGET_PROPERTIES(
  ${COMPONENT_NAME} PROPERTIES
  VERSION ${COMPONENT_VERSION}
  SOVERSION ${COMPONENT_VERSION}
  )

TARGET_INCLUDE_DIRECTORIES(${COMPONENT_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>/${COMPONENT_PATH}
  ${PISTACHE_INCLUDE_DIRS}
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
  )

TARGET_COMPILE_DEFINITIONS(${COMPONENT_NAME}
  PUBLIC ${CPP_DEFS}
  )

TARGET_COMPILE_OPTIONS(${COMPONENT_NAME}
  PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:${CUSTOM_CXX_FLAGS}>
  $<$<COMPILE_LANGUAGE:C>:${CUSTOM_C_FLAGS}>
  )

TARGET_LINK_LIBRARIES(${COMPONENT_NAME}
  PUBLIC
  Qt5::Core
  Qt5::Network
  ${CMAKE_THREAD_LIBS_INIT}
  ${PISTACHE_LIBRARIES}
  PRIVATE
  ${CMAKE_THREAD_LIBS_INIT}
  )

#-----
# Install
#-----
include(InstallLibraryComponents)

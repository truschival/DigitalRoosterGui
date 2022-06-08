# SPDX-License-Identifier: GPL-3.0-or-later
#
# copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
# Licensed under GNU PUBLIC LICENSE Version 3 or later

MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

# name of library (without lib- prefix)
string(TOLOWER ${PROJECT_NAME} LIBRARY_NAME)
# Component name (what to build and install) used in add_executable or add_library
set(COMPONENT_NAME ${LIBRARY_NAME})
# Interface/binary version
SET(COMPONENT_VERSION ${PROJECT_VERSION})
#includes etc. in folder (no subfolder here)
SET(COMPONENT_PATH "")

SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

# QT5 Components used in library
find_package(Qt5 COMPONENTS Core Multimedia Network REQUIRED)

#------------------------------------------------------------------------------
# Add dependencies
#------------------------------------------------------------------------------

SET(OTHER_LIBS "")

# target wpa_ctrl always exists (the content depends on HAS_WPA_SUPPLICANT)
IF(TARGET wpa_ctrl)
  add_library(${PROJECT_NAME}::wpa_ctrl ALIAS wpa_ctrl)
ELSE()
  find_package(${PROJECT_NAME} COMPONENTS wpa_ctrl CONFIG REQUIRED)
ENDIF()
LIST(APPEND OTHER_LIBS wpa_ctrl)

#------------------------------
# add compile definitions
#------------------------------
SET(CPP_DEFS "")
LIST(APPEND CPP_DEFS GIT_DESCRIBE_REV=${GIT_DESCRIBE_REV} )

#------------------------------
# normal sources
#------------------------------

SET(SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/playableitem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/configuration.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/rss2podcastsource.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PodcastSource.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/httpclient.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/UpdateTask.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/alarm.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mediaplayer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mediaplayerproxy.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/alarmmonitor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/alarmdispatcher.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/weather.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/powercontrol.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/brightnesscontrol.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/volume_button.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/podcast_serializer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/wifi_control.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sleeptimer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/util.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/hardware_configuration.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/networkinfo.cpp
	)

#------------------------------
# QT MOC, UIC RCC input
#------------------------------

# Moc any classes derived from QObject
SET(MOC_SRC
	${PROJECT_INCLUDE_DIR}/configuration.hpp
	${PROJECT_INCLUDE_DIR}/httpclient.hpp
	${PROJECT_INCLUDE_DIR}/UpdateTask.hpp
	${PROJECT_INCLUDE_DIR}/PlayableItem.hpp
	${PROJECT_INCLUDE_DIR}/PodcastSource.hpp
	${PROJECT_INCLUDE_DIR}/alarm.hpp
	${PROJECT_INCLUDE_DIR}/mediaplayer.hpp
	${PROJECT_INCLUDE_DIR}/mediaplayerproxy.hpp
	${PROJECT_INCLUDE_DIR}/alarmdispatcher.hpp
	${PROJECT_INCLUDE_DIR}/alarmmonitor.hpp
	${PROJECT_INCLUDE_DIR}/weather.hpp
	${PROJECT_INCLUDE_DIR}/powercontrol.hpp
	${PROJECT_INCLUDE_DIR}/brightnesscontrol.hpp
	${PROJECT_INCLUDE_DIR}/volume_button.hpp
	${PROJECT_INCLUDE_DIR}/podcast_serializer.hpp
	${PROJECT_INCLUDE_DIR}/wifi_control.hpp
	${PROJECT_INCLUDE_DIR}/sleeptimer.hpp
	${PROJECT_INCLUDE_DIR}/networkinfo.hpp
	${PROJECT_INCLUDE_DIR}/IHardware.hpp
	)

# Use real hardware control or stub
if(${SYSTEM_TARGET_NAME} STREQUAL "Embedded" )
	LIST(APPEND SRCS	${CMAKE_CURRENT_SOURCE_DIR}/hardwarecontrol_mk3.cpp )
	LIST(APPEND MOC_SRC	${PROJECT_INCLUDE_DIR}/hardwarecontrol_mk3.hpp )
else()
	LIST(APPEND SRCS	${CMAKE_CURRENT_SOURCE_DIR}/hardwarecontrol_stub.cpp )
	LIST(APPEND MOC_SRC	${PROJECT_INCLUDE_DIR}/hardwarecontrol_stub.hpp )
	LIST(APPEND CPP_DEFS HARDWARE_STUB )
endif()

#------------------------------
# Output a library
#------------------------------
ADD_LIBRARY(${COMPONENT_NAME} STATIC
	${SRCS}
	${HW_DEP_SRCS}
	${MOC_SRC}
	)

SET_TARGET_PROPERTIES(${COMPONENT_NAME} 
  PROPERTIES
  VERSION ${COMPONENT_VERSION}
  SOVERSION ${COMPONENT_VERSION}
  )

TARGET_INCLUDE_DIRECTORIES(${COMPONENT_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
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

target_link_libraries(${COMPONENT_NAME}
  PUBLIC
  Qt5::Core
  Qt5::Multimedia
  ${OTHER_LIBS}
  PRIVATE
  Qt5::Core
  Qt5::Multimedia
  Qt5::Network
  )

#------------------------------
# Install
#------------------------------
include(InstallLibraryComponents)

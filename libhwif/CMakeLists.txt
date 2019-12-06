MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

# name of library (without lib- prefix)
SET(LIBRARY_NAME hwif)
# Component name (what to build and install) used in add_executable or add_library
set(COMPONENT_NAME ${LIBRARY_NAME})
# Interface/binary version
SET(COMPONENT_VERSION ${PROJECT_VERSION})
#includes etc. in folder
SET(COMPONENT_PATH ${LIBRARY_NAME})

# QT5 Components used in library
find_package(Qt5 COMPONENTS Core)

#------------------------------
# add compile definitions
#------------------------------
SET(CPP_DEFS "")
LIST(APPEND CPP_DEFS SYSTEM_TARGET_NAME=${SYSTEM_TARGET_NAME} )

#------------------------------
# normal sources
#------------------------------
set(SRCS ${CMAKE_CURRENT_SOURCE_DIR}/hardware_configuration.cpp )

if(${SYSTEM_TARGET_NAME} STREQUAL "RPi" )
	MESSAGE(STATUS "adding raspberry pi specific flags")
	LIST(APPEND SRCS ${CMAKE_CURRENT_SOURCE_DIR}/hardwarecontrol_rpi.cpp )
	find_library(WIRING_LIB 
		wiringPi
		DOC "Wiring Pi library"
	)
	MESSAGE(STATUS "Found library  ${WIRING_LIB-VERSION} : ${WIRING_LIB} ")
else()
	LIST(APPEND SRCS ${CMAKE_CURRENT_SOURCE_DIR}/hardwarecontrol_stub.cpp )
endif()

#------------------------------
# QT MOC, UIC RCC input
#------------------------------

# Moc any classes derived from QObject
SET(MOC_SRCS
    ${PROJECT_INCLUDE_DIR}/hwif/hardware_control.hpp
    )


#------------------------------
# Output a library
#------------------------------
ADD_LIBRARY(${COMPONENT_NAME} STATIC 
	${SRCS} 
	${MOC_SRCS}
	)

SET_TARGET_PROPERTIES(${COMPONENT_NAME} 
  PROPERTIES
  VERSION ${COMPONENT_VERSION}
  SOVERSION ${COMPONENT_VERSION}
  )

TARGET_INCLUDE_DIRECTORIES(${COMPONENT_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>/${COMPONENT_PATH}
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

target_link_libraries(
	${COMPONENT_NAME}
	PRIVATE
  	Qt5::Core
	)

if(NOT ${SYSTEM_TARGET_NAME} STREQUAL "Host" )
	target_link_libraries(
		${COMPONENT_NAME}
		PUBLIC
	  	${WIRING_LIB}
	  )
endif()

#-----
# Install
#-----
include(InstallLibraryComponents)


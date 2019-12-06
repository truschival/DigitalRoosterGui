MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

# name of library (without lib- prefix)
SET(LIBRARY_NAME wpa_ctrl)
# Component name (what to build and install) used in add_executable or add_library
set(COMPONENT_NAME ${LIBRARY_NAME})
# Interface/binary version
SET(COMPONENT_VERSION ${PROJECT_VERSION})
#includes etc. in folder
SET(COMPONENT_PATH ${LIBRARY_NAME})

#------------------------------
# add compile definitions
#------------------------------
SET(CPP_DEFS "CONFIG_CTRL_IFACE=1")

if(UNIX)
  LIST(APPEND CPP_DEFS
    CONFIG_CTRL_IFACE_UNIX=1)
endif (UNIX)

#------------------------------
# normal sources
#------------------------------
set(SRCS  "")

if(HAS_WPA_SUPPLICANT)
  LIST(APPEND SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/wpa_ctrl.c)
else()
  LIST(APPEND SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/wpa_ctrl_stub.c)
endif()

#------------------------------
# Output a library
#------------------------------
ADD_LIBRARY(${COMPONENT_NAME}
  STATIC
  ${SRCS}
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
  PUBLIC ""
  PRIVATE ${CPP_DEFS}
  )

TARGET_COMPILE_OPTIONS(${COMPONENT_NAME}
  PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:${CUSTOM_CXX_FLAGS}>
  $<$<COMPILE_LANGUAGE:C>:${CUSTOM_C_FLAGS}>
  )

#-----
# Install
#-----
include(InstallLibraryComponents)

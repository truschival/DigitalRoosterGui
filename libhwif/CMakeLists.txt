MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

# name of library (without lib- prefix)
SET(LIBRARY_NAME hwif)

#Since the "standard" cmake template works with component name set it
set(COMPONENT_NAME "HWIF")

#includes etc. in folder
SET(COMPONENT_PATH ${LIBRARY_NAME})

set(version_config "${GENERATED_DIR}/${COMPONENT_NAME}ConfigVersion.cmake")
set(component_config "${GENERATED_DIR}/${COMPONENT_NAME}Config.cmake")
set(targets_export_name "${COMPONENT_NAME}Targets")

# Interface/binary version
SET(LIBRARY_VERSION ${PROJECT_VERSION})

#------------------------------
# add compile definitions
#------------------------------
SET(CPP_DEFS "")
LIST(APPEND CPP_DEFS SYSTEM_TARGET_NAME=${SYSTEM_TARGET_NAME} )

#------------------------------
# normal sources
#------------------------------

if(${SYSTEM_TARGET_NAME} STREQUAL "RPi" )
	MESSAGE(STATUS "adding raspberry pi specific flags")
	set(SRCS ${CMAKE_CURRENT_SOURCE_DIR}/hardwarecontrol_rpi.c )
	find_library(WIRING_LIB 
		wiringPi
		DOC "Wiring Pi library"
	)
	MESSAGE(STATUS "Found library  ${WIRING_LIB-VERSION} : ${WIRING_LIB} ")
else()
	set(SRCS ${CMAKE_CURRENT_SOURCE_DIR}/hardwarecontrol_stub.c )
endif()

#------------------------------
# Output a library
#------------------------------
ADD_LIBRARY(${LIBRARY_NAME} STATIC 
	${SRCS} 
	)

SET_TARGET_PROPERTIES(
  ${LIBRARY_NAME} PROPERTIES
  VERSION ${LIBRARY_VERSION}
  SOVERSION ${LIBRARY_VERSION}
  )

TARGET_INCLUDE_DIRECTORIES(
  ${LIBRARY_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>/hal
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
  )

TARGET_COMPILE_DEFINITIONS(${LIBRARY_NAME}
  PUBLIC ${CPP_DEFS} 
  )

TARGET_COMPILE_OPTIONS(${LIBRARY_NAME} PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:${CUSTOM_CXX_FLAGS}>
  $<$<COMPILE_LANGUAGE:C>:${CUSTOM_C_FLAGS}>
  )

if(NOT ${SYSTEM_TARGET_NAME} STREQUAL "Host" )
	target_link_libraries(
	  ${LIBRARY_NAME}
	  PUBLIC
	  ${WIRING_LIB}
	  )
endif()

#-----
# Install
#-----
MESSAGE(STATUS "** Generating Package Configurations **")

include(CMakePackageConfigHelpers)
WRITE_BASIC_PACKAGE_VERSION_FILE(
  ${version_config}
  VERSION ${LIBRARY_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Note: variable 'targets_export_name' used
CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/cmake/Config.cmake.in"
  "${component_config}" @ONLY)

INSTALL(TARGETS ${LIBRARY_NAME}
  EXPORT ${targets_export_name}
  COMPONENT DEVELOP
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${INSTALL_BIN_DIR}
  # this will add -I<prefix>include/transmog to client compile flags
  #INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}/${COMPONENT_PATH}
  INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
  )

INSTALL(DIRECTORY
  ${PROJECT_INCLUDE_DIR}/${COMPONENT_PATH}
  COMPONENT DEVELOP
  DESTINATION ${INSTALL_INCLUDE_DIR}
)

INSTALL(
  EXPORT ${targets_export_name}
  COMPONENT DEVELOP
  NAMESPACE "${COMPONENT_NAME}::"
  DESTINATION "${INSTALL_CMAKE_DIR}/${COMPONENT_NAME}"
  )

INSTALL(
  FILES "${component_config}" "${version_config}"
  COMPONENT DEVELOP
  DESTINATION "${INSTALL_CMAKE_DIR}/${COMPONENT_NAME}"
  )


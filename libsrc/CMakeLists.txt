MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

# name of library (without lib- prefix)
string(TOLOWER ${PROJECT_NAME} LIBRARY_NAME)

#Since the "standard" cmake template works with component name set it
set(COMPONENT_NAME ${PROJECT_NAME})

#includes etc. in folder
SET(COMPONENT_PATH ${LIBRARY_NAME})

set(version_config "${GENERATED_DIR}/${COMPONENT_NAME}ConfigVersion.cmake")
set(component_config "${GENERATED_DIR}/${COMPONENT_NAME}Config.cmake")
set(targets_export_name "${COMPONENT_NAME}Targets")

# Interface/binary version
SET(LIBRARY_VERSION ${PROJECT_VERSION})

# add . to the includes during the build
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5 COMPONENTS Core Multimedia Network REQUIRED)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# --------------------------------------------------------------------------------
# Add dependencies
# --------------------------------------------------------------------------------

SET(OTHER_LIBS "")
IF(TARGET hwif)
	add_library(${PROJECT_NAME}::hwif ALIAS hwif)
ELSE()
	find_package(${PROJECT_NAME} COMPONENTS hwif CONFIG REQUIRED)
ENDIF()
LIST(APPEND OTHER_LIBS hwif)

#------------------------------
# add compile definitions
#------------------------------
SET(CPP_DEFS "")
LIST(APPEND CPP_DEFS GIT_DESCRIBE_REV=${GIT_DESCRIBE_REV} )

#------------------------------
# normal sources
#------------------------------

set(SRCS
   ${CMAKE_CURRENT_SOURCE_DIR}/configuration_manager.cpp
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
)

# Moc any classes derived from QObject
qt5_wrap_cpp(MOC_SRC
        ${PROJECT_INCLUDE_DIR}/configuration_manager.hpp
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
        )

#------------------------------
# Output a library
#------------------------------
ADD_LIBRARY(${LIBRARY_NAME} STATIC
	${SRCS}
	${HW_DEP_SRCS}
	${MOC_SRC}
	)

SET_TARGET_PROPERTIES(
  ${LIBRARY_NAME} PROPERTIES
  VERSION ${LIBRARY_VERSION}
  SOVERSION ${LIBRARY_VERSION}
  )

TARGET_INCLUDE_DIRECTORIES(
  ${LIBRARY_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
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

target_link_libraries(
  ${LIBRARY_NAME}
  PUBLIC
  Qt5::Core
  Qt5::Multimedia
  ${OTHER_LIBS}
  PRIVATE
  Qt5::Core
  Qt5::Multimedia
  Qt5::Network
  )


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
  ${PROJECT_INCLUDE_DIR}/
  COMPONENT DEVELOP
  DESTINATION ${INSTALL_INCLUDE_DIR}/${COMPONENT_PATH}
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

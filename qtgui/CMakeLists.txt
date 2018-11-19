MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")
SET(BINARY_NAME "DigitalRoosterGui")

find_package(Qt5 COMPONENTS Core Widgets Multimedia Qml Quick REQUIRED)
MESSAGE(STATUS "Found Qt5 version: ${Qt5Widgets_VERSION_STRING}")

#Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --------------------------------------------------------------------------------
# C++ Srcs
# --------------------------------------------------------------------------------
SET(SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/podcastsourcemodel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/podcastepisodemodel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/iradiolistmodel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/alarmlistmodel.cpp
)

# --------------------------------------------------------------------------------
# App Depends on library
# --------------------------------------------------------------------------------

SET(PROJECT_CORE_LIB "")
if(TARGET digitalrooster)
  add_library(${PROJECT_NAME}::digitalrooster ALIAS digitalrooster)
else()
    find_package(${PROJECT_NAME} COMPONENTS digitalrooster CONFIG REQUIRED)
endif()
LIST(APPEND PROJECT_CORE_LIB digitalrooster)

#------------------------------
# add compile definitions
#------------------------------
SET(CPP_DEFS "")
LIST(APPEND CPP_DEFS "AWESOME=1" )

#------------------------------
# QT magic
#------------------------------
qt5_wrap_ui(UI_HDRS
  )

SET(QRC_SRC
  qml.qrc
  externalRes/external.qrc
  )

#------------------------------
# Binary
#------------------------------
# Executable (for target)
add_executable(${BINARY_NAME}
  ${SRCS}
  ${UI_HDRS}
  ${MOC_SRC}
  ${QRC_SRC}
  )


SET_TARGET_PROPERTIES(
  ${BINARY_NAME}
  PROPERTIES
  VERSION ${PROJECT_VERSION}
  DEBUG_POSTFIX "_dbg"
  )

TARGET_COMPILE_DEFINITIONS(${BINARY_NAME}
  # no public flags, nobody should use the main app as dependency
  PRIVATE 
  ${CPP_DEFS} 
  )

TARGET_COMPILE_OPTIONS(${BINARY_NAME} PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:${CUSTOM_CXX_FLAGS}>
  $<$<COMPILE_LANGUAGE:C>:${CUSTOM_C_FLAGS}>
  )

# Linkage with gtest_main coverage etc.
TARGET_LINK_LIBRARIES(
  ${BINARY_NAME}
  PRIVATE
  ${PROJECT_CORE_LIB}
  Qt5::Quick
  Qt5::Qml
  ${CUSTOM_LINK_FLAGS}
  )

#-----
# Install
#-----
MESSAGE(STATUS "** Generating Package Configurations **")

INSTALL(TARGETS ${BINARY_NAME}
  EXPORT ${targets_export_name}
  COMPONENT APPLICATION
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${INSTALL_BIN_DIR}
  INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
  )

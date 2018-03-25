MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")
SET(BINARY_NAME "DigitalRoosterGui")

# Threading library for gtest
# Use ${CMAKE_THREAD_LIBS_INIT} for the library
find_package(Threads REQUIRED)

find_package(Qt5 COMPONENTS Core Widgets Multimedia Qml Quick REQUIRED)

#find_package(Qt5Widgets REQUIRED)
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
  COMPILE_DEFINITIONS ${CPP_DEFS}
  VERSION ${PROJECT_VERSION}
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
  DEBUG_POSTFIX "_dbg"
  )


# Linkage with gtest_main coverage etc.
TARGET_LINK_LIBRARIES(
  ${BINARY_NAME}
  ${PROJECT_CORE_LIB}
  Qt5::Quick
  Qt5::Qml
  )

if(TEST_COVERAGE)
  TARGET_LINK_LIBRARIES (${BINARY_NAME}  ${COVERAGE_LIB} )
endif(TEST_COVERAGE)

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


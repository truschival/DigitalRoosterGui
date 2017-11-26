MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")
SET(BINARY_NAME "DigitalRoosterGui")

# Threading library for gtest
# Use ${CMAKE_THREAD_LIBS_INIT} for the library
find_package(Threads REQUIRED)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
MESSAGE(STATUS "Found Qt5 version: ${Qt5Widgets_VERSION_STRING}")
include_directories(${Qt5Widgets_INCLUDES})

#Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)


# --------------------------------------------------------------------------------
# C++ Srcs
# --------------------------------------------------------------------------------
SET(SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/graphwidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/node.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/edge.cpp
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
SET(CPP_DEFS "AWESOMEVAR=1" Foo )
LIST(APPEND CPP_DEFS
  ${Qt5Widgets_COMPILE_DEFINITIONS}
  )

#------------------------------
# QT magic
#------------------------------
# qt5_wrap_ui(UI_HDRS
#   ${PROJECT_UI_DIR}/mainwindow.ui
#   )

qt5_wrap_cpp(MOC_SRC
#  graphwidget.h
#  node.h
#  edge.h
  )

#------------------------------
# Binary
#------------------------------

# UnitTests Executable (for target)
add_executable(${BINARY_NAME}
  ${SRCS}
  ${UI_HDRS}
  ${MOC_SRC}
  )


SET_TARGET_PROPERTIES(
  ${BINARY_NAME}
  PROPERTIES
  COMPILE_DEFINITIONS ${CPP_DEFS}
  CXX_STANDARD 11
  VERSION ${PROJECT_VERSION}
  DEBUG_POSTFIX "_dbg"
  )


# Linkage with gtest_main coverage etc.
TARGET_LINK_LIBRARIES(
  ${BINARY_NAME}
  ${PROJECT_CORE_LIB}   # Units under test
  Qt5::Widgets
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


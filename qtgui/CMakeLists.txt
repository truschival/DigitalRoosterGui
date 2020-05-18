MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

SET(BINARY_NAME "DigitalRoosterGui")
# Component name (what to build and install) used in add_executable or add_library
set(COMPONENT_NAME ${BINARY_NAME})
# Interface/binary version
SET(COMPONENT_VERSION ${PROJECT_VERSION})
# Gui has extra QT dependencies: QML and QTquick
find_package(Qt5 COMPONENTS Qml Quick REQUIRED)
# QTQuickCompiler for compile-time QML
find_package(Qt5QuickCompiler)

#Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#-------------------------------------------------------------------------------
# App Depends on library
#-------------------------------------------------------------------------------
SET(OTHER_LIBS "")
SET(PROJECT_CORE_LIB "")
if(TARGET digitalrooster)
  add_library(${PROJECT_NAME}::digitalrooster ALIAS digitalrooster)
else()
  find_package(${PROJECT_NAME} COMPONENTS digitalrooster CONFIG REQUIRED)
endif()
LIST(APPEND PROJECT_CORE_LIB digitalrooster)

if(REST_API)
  if(TARGET restconfig)
    add_library(${PROJECT_NAME}::restconfig ALIAS restconfig)
  else()
    find_package(${PROJECT_NAME} COMPONENTS restconfig CONFIG REQUIRED)
  endif()
  LIST(APPEND REST_LIB restconfig)
endif(REST_API)

#-------------------------------------------------------------------------------
# C++ Srcs
#-------------------------------------------------------------------------------
SET(SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/podcastsourcemodel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/podcastepisodemodel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/iradiolistmodel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/alarmlistmodel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/wifilistmodel.cpp
  )

#------------------------------
# QT MOC, UIC RCC input
#------------------------------
SET(UI_HDRS
  )

SET(MOC_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/podcastsourcemodel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/podcastepisodemodel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/iradiolistmodel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/alarmlistmodel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/wifilistmodel.hpp
  )

# Data resources (fonts,images etc.)
qt5_add_resources(QRC_DATA externalRes/external.qrc)
# QML sources for Qt5QuickCompiler
qtquick_compiler_add_resources(QML_QRC_OBJECTS qml/qml.qrc)

#------------------------------
# add compile definitions
#------------------------------
SET(CPP_DEFS "")
LIST(APPEND CPP_DEFS "" )

if(REST_API)
  LIST(APPEND CPP_DEFS "REST_API")
endif(REST_API)

#------------------------------
# Binary
#------------------------------
# Executable (for target)
ADD_EXECUTABLE(${COMPONENT_NAME}
  ${SRCS}
  ${UI_HDRS}
  ${MOC_SRC}
  ${QML_QRC_OBJECTS}
  ${QRC_DATA}
  )

SET_TARGET_PROPERTIES(${COMPONENT_NAME}
  PROPERTIES
  VERSION ${COMPONENT_VERSION}
  )

TARGET_COMPILE_DEFINITIONS(${COMPONENT_NAME}
  # no public flags, nobody should use the main app as dependency
  PRIVATE
  ${CPP_DEFS}
  )

TARGET_COMPILE_OPTIONS(${COMPONENT_NAME}
  PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:${CUSTOM_CXX_FLAGS}>
  $<$<COMPILE_LANGUAGE:C>:${CUSTOM_C_FLAGS}>
  )

# Linkage with gtest_main coverage etc.
TARGET_LINK_LIBRARIES(${COMPONENT_NAME}
  PRIVATE
  ${PROJECT_CORE_LIB}
  ${REST_LIB}
  Qt5::Quick
  Qt5::Qml
  ${CUSTOM_LINK_FLAGS}
  )

#-----
# Install
#-----
include(InstallLibraryComponents)

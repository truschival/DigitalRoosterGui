MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

find_package(Qt5 COMPONENTS Core REQUIRED)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#------------------------------
# QT magic
#------------------------------

SET(QRC_EXTERNAL_RES
  "${CMAKE_CURRENT_SOURCE_DIR}/qml_external.qrc"
  PARENT_SCOPE
  )

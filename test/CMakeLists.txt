MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

SET(BINARY_NAME "${PROJECT_NAME}_gtest")

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
find_package(Qt5 COMPONENTS Core Multimedia Test REQUIRED)

# Threading library for gtest
# Use ${CMAKE_THREAD_LIBS_INIT} for the library
find_package(Threads REQUIRED)

# --------------------------------------------------------------------------------
# Unit test sources, TestDoubles, Mocks etc.
# --------------------------------------------------------------------------------
SET(TEST_HARNESS_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_confmon.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_settings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_podcast_reader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_update_task.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_podcastsource.cpp
  )

# Moc any classes derived from QObject
qt5_wrap_cpp(MOC_SRC

)

# --------------------------------------------------------------------------------
# Add libraries to test here
# --------------------------------------------------------------------------------

SET(DUT_LIBS "")
if(TARGET digitalrooster)
  add_library(${PROJECT_NAME}::digitalrooster ALIAS digitalrooster)
else()
    find_package(${PROJECT_NAME} COMPONENTS digitalrooster CONFIG REQUIRED)
endif()
LIST(APPEND DUT_LIBS digitalrooster)

#------------------------------
# add compile definitions
#------------------------------
SET(CPP_DEFS "")
LIST(APPEND CPP_DEFS "")

# --------------------------------------------------------------------------------
if(NOT BUILD_GTEST_FROM_SRC)
  # Use prebuilt google_test
  find_package(GTest REQUIRED)

else(NOT BUILD_GTEST_FROM_SRC)
  include(ExternalProject)

  #--------------------------------------------------------------------------
  # Google Test as external project
  # Why - see http://code.google.com/p/googletest/wiki/FAQ#
  # How - borrowed form stackexchange
  #		(http://stackoverflow.com/questions/9689183/cmake-googletest)
  # http://stackoverflow.com/questions/9689183/cmake-googletest
  # http://www.kitware.com/media/html/BuildingExternalProjectsWithCMake2.8.html
  # https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project
  #--------------------------------------------------------------------------
  set(GTEST_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/GTestExternal/")
  
  # Download and unpack googletest at configure time
  configure_file(CMakeLists.txt.in ${GTEST_PREFIX}/CMakeLists.txt)

  message(STATUS "Executing cmake on generated Download CMakeLists.txt") 
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"  .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${GTEST_PREFIX} )
  if(result)
  	message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()

  message(STATUS "Building google test" ) 
  execute_process(COMMAND ${CMAKE_COMMAND} --build  ${GTEST_PREFIX}
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${GTEST_PREFIX}/googletest-build )
  if(result)
	message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(
	${GTEST_PREFIX}/googletest-src
    ${GTEST_PREFIX}/googletest-build
    EXCLUDE_FROM_ALL)

  set(LIBPREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}")
  set(LIBSUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")

  set(GTEST_INCLUDES "${GTEST_PREFIX}/googletest-src/googletest/include")
  set(GMOCK_INCLUDES "${GTEST_PREFIX}/googletest-src/googlemock/include")
  
  set(GTEST_LIBRARY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${LIBPREFIX}gtest")
  set(GTEST_MAINLIB  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${LIBPREFIX}gtest_main")

  set(GMOCK_LIBRARY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${LIBPREFIX}gmock")
  set(GMOCK_MAINLIB  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${LIBPREFIX}gmock_main")
  
  add_library(GTest IMPORTED STATIC GLOBAL)
  set_target_properties(GTest PROPERTIES
  	IMPORTED_LOCATION "${GTEST_LIBRARY}${LIBSUFFIX}"
   	IMPORTED_LOCATION_DEBUG  "${GTEST_LIBRARY}${CMAKE_DEBUG_POSTFIX}${LIBSUFFIX}"
  	INTERFACE_INCLUDE_DIRECTORIES     "${GTEST_INCLUDES};${GMOCK_INCLUDES}"
  	IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")

  add_library(GTestMain IMPORTED STATIC GLOBAL)
  set_target_properties(GTestMain PROPERTIES
 	IMPORTED_LOCATION "${GTEST_MAINLIB}${LIBSUFFIX}"
   	IMPORTED_LOCATION_DEBUG  "${GTEST_MAINLIB}${CMAKE_DEBUG_POSTFIX}${LIBSUFFIX}"
 	INTERFACE_INCLUDE_DIRECTORIES      "${GTEST_INCLUDES}"
  	IMPORTED_LINK_INTERFACE_LIBRARIES  GTest)

  
  add_library(GMock IMPORTED STATIC GLOBAL)
  set_target_properties(GMock PROPERTIES
	IMPORTED_LOCATION "${GMOCK_LIBRARY}${LIBSUFFIX}"
   	IMPORTED_LOCATION_DEBUG  "${GMOCK_LIBRARY}${CMAKE_DEBUG_POSTFIX}${LIBSUFFIX}"
	DEBUG_POSTFIX "_dbg"
   	INTERFACE_INCLUDE_DIRECTORIES     "${GTEST_INCLUDES};${GMOCK_INCLUDES}"
   	IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")

  add_library(GMockMain IMPORTED STATIC GLOBAL)
  set_target_properties(GMockMain PROPERTIES
	IMPORTED_LOCATION "${GMOCK_MAINLIB}${LIBSUFFIX}"
   	IMPORTED_LOCATION_DEBUG  "${GMOCK_MAINLIB}${CMAKE_DEBUG_POSTFIX}${LIBSUFFIX}"
	DEBUG_POSTFIX "_dbg"
   	INTERFACE_INCLUDE_DIRECTORIES      "${GTEST_INCLUDES};${GMOCK_INCLUDES}"
   	IMPORTED_LINK_INTERFACE_LIBRARIES  GMock )
  
  add_dependencies(GTest gtest_main gtest)
  add_dependencies(GMock gmock_main gmock)
 
endif(NOT BUILD_GTEST_FROM_SRC)


#------------------------------
# Binary
#------------------------------
SET(RSS_FILE_GOOD "alternativlos.rss")
SET(RSS_FILE_BAD "alternativlos_bad.rss")
SET(RSS_FILE_MALFORMAT "alternativlos_malformatted.rss")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${RSS_FILE_GOOD}" "${CMAKE_BINARY_DIR}/${RSS_FILE_GOOD}" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${RSS_FILE_BAD}" "${CMAKE_BINARY_DIR}/${RSS_FILE_BAD}" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${RSS_FILE_MALFORMAT}" "${CMAKE_BINARY_DIR}/${RSS_FILE_MALFORMAT}" COPYONLY)

# UnitTests Executable (for target)
add_executable(${BINARY_NAME}
  ${TEST_HARNESS_SRCS}
  ${UI_HDRS}
  ${MOC_SRC}
  )

SET_TARGET_PROPERTIES(
  ${BINARY_NAME}
  PROPERTIES
  COMPILE_DEFINITIONS "${CPP_DEFS}"
  VERSION ${PROJECT_VERSION}
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
  )

# Linkage with gtest_main coverage etc.
TARGET_LINK_LIBRARIES(
  ${BINARY_NAME}
  ${DUT_LIBS}   # Units under test
  GMock # main not required, implemented in test.cpp
  Qt5::Test
  )

if(TEST_COVERAGE)
  #Remove googletest, autogenerated QT code and headers for testcovera
  set(COVERAGE_EXCLUDES '*GTestExternal/*' "*_autogen*" '/usr/*')
  
  SETUP_TARGET_FOR_COVERAGE(
    NAME ${BINARY_NAME}_coverage                # New target name
    EXECUTABLE ${BINARY_NAME} -j 4 # Executable in PROJECT_BINARY_DIR
    DEPENDENCIES ${BINARY_NAME})
endif(TEST_COVERAGE) 

# Call the testBinary with junit-xml output
add_test(junitout "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BINARY_NAME}"
  --gtest_output=xml:gtestresults.xml )

if(INSTALL_UNIT_TEST_ON_TARGET)
  INSTALL(TARGETS  ${TEST_BINARY_NAME} RUNTIME DESTINATION ${TARGET_EXE_INSTALL_DIR} )
endif(INSTALL_UNIT_TEST_ON_TARGET)

MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

SET(GEST_BINARY_NAME "${PROJECT_NAME}_gtest")
SET(RESTSERVER_NAME "restserver")

set(COMPONENT_NAME "DigitalRooster-Test")
# Interface/binary version
SET(COMPONENT_VERSION ${PROJECT_VERSION})

# Extra QT5 components for tests
find_package(Qt5 COMPONENTS Test REQUIRED)

# Threading library for gtest
# Use ${CMAKE_THREAD_LIBS_INIT} for the library
find_package(Threads REQUIRED)

#-------------------------------------------------------------------------------
# Add libraries to test here
#-------------------------------------------------------------------------------

SET(DUT_LIBS "")
IF(TARGET digitalrooster)
	add_library(${PROJECT_NAME}::digitalrooster ALIAS digitalrooster)
ELSE()
	find_package(${PROJECT_NAME} COMPONENTS digitalrooster CONFIG REQUIRED)
ENDIF()
LIST(APPEND DUT_LIBS digitalrooster)

IF(UNIX)
	# target wpa_ctrl only exist ifdef UNIX
	IF(TARGET wpa_ctrl)
		add_library(${PROJECT_NAME}::wpa_ctrl ALIAS wpa_ctrl)
	ELSE()
		find_package(${PROJECT_NAME} COMPONENTS wpa_ctrl CONFIG REQUIRED)
	ENDIF()
	LIST(APPEND DUT_LIBS wpa_ctrl)
ENDIF(UNIX)

if(REST_API)
  if(TARGET restconfig)
    add_library(${PROJECT_NAME}::restconfig ALIAS restconfig)
  else()
    find_package(${PROJECT_NAME} COMPONENTS restconfig CONFIG REQUIRED)
  endif()
  LIST(APPEND DUT_LIBS restconfig)
endif(REST_API)

#------------------------------
# add compile definitions
#------------------------------
SET(CPP_DEFS "")
LIST(APPEND CPP_DEFS "")

#-------------------------------------------------------------------------------
IF(NOT BUILD_GTEST_FROM_SRC)
  # Use prebuilt google_test
  find_package(GTest REQUIRED)

ELSE(NOT BUILD_GTEST_FROM_SRC)
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
   	INTERFACE_INCLUDE_DIRECTORIES     "${GTEST_INCLUDES};${GMOCK_INCLUDES}"
   	IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")

  add_library(GMockMain IMPORTED STATIC GLOBAL)
  set_target_properties(GMockMain PROPERTIES
	IMPORTED_LOCATION "${GMOCK_MAINLIB}${LIBSUFFIX}"
   	IMPORTED_LOCATION_DEBUG  "${GMOCK_MAINLIB}${CMAKE_DEBUG_POSTFIX}${LIBSUFFIX}"
   	INTERFACE_INCLUDE_DIRECTORIES      "${GTEST_INCLUDES};${GMOCK_INCLUDES}"
   	IMPORTED_LINK_INTERFACE_LIBRARIES  GMock )

  add_dependencies(GTest gtest_main gtest)
  add_dependencies(GMock gmock_main gmock)

endif(NOT BUILD_GTEST_FROM_SRC)

#------------------------------
# Test input files
#------------------------------

SET(RSS_FILE_GOOD "alternativlos.rss")
SET(RSS_FILE_BAD "alternativlos_bad.rss")
SET(RSS_FILE_MALFORMAT "alternativlos_malformatted.rss")
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/${RSS_FILE_GOOD}"
  "${CMAKE_BINARY_DIR}/${RSS_FILE_GOOD}"
  COPYONLY)
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/${RSS_FILE_BAD}"
  "${CMAKE_BINARY_DIR}/${RSS_FILE_BAD}"
  COPYONLY)
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/${RSS_FILE_MALFORMAT}"
  "${CMAKE_BINARY_DIR}/${RSS_FILE_MALFORMAT}"
  COPYONLY)

SET(AUDIO_TEST_FILE "testaudio.mp3")
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/${AUDIO_TEST_FILE}"
  "${CMAKE_BINARY_DIR}/${AUDIO_TEST_FILE}"
  COPYONLY)

SET(WEATHER_TEST_FILE "sample_weather.json")
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/${WEATHER_TEST_FILE}"
  "${CMAKE_BINARY_DIR}/${WEATHER_TEST_FILE}"
  COPYONLY)

SET(FORECAST_TEST_FILE "sample_forecast.json")
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/${FORECAST_TEST_FILE}"
  "${CMAKE_BINARY_DIR}/${FORECAST_TEST_FILE}"
  COPYONLY)

#-------------------------------------------------------------------------------
# Unit test sources, TestDoubles, Mocks etc.
#-------------------------------------------------------------------------------
SET(TEST_HARNESS_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/testcommon.cpp		
  ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_settings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_podcast_reader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_update_task.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_podcastsource.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_alarm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_mediaplayerproxy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_alarmdispatcher.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_alarmmonitor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_weather.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_brightness.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_playableitem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_volume_button.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_podcast_serializer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_sleeptimer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_powercontrol.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_hardware_config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test_restapihandler.cpp
  )

IF(HAS_WPA_SUPPLICANT)
	LIST(APPEND TEST_HARNESS_SRCS
		${CMAKE_CURRENT_SOURCE_DIR}/test_wpa_ctrl.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/test_wifi_control.cpp
	)
ENDIF()

#------------------------------
# Gtest binary
#------------------------------

# Standard unit test binary using google test
add_executable(${GEST_BINARY_NAME}
  ${TEST_HARNESS_SRCS}
  )

SET_TARGET_PROPERTIES(${GEST_BINARY_NAME}
  PROPERTIES
  VERSION ${PROJECT_VERSION}
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
  )

TARGET_COMPILE_DEFINITIONS(${GEST_BINARY_NAME}
  PUBLIC ${CPP_DEFS} TEST_LOG_NAME=${GEST_BINARY_NAME}.log
  )

TARGET_COMPILE_OPTIONS(${GEST_BINARY_NAME}
  PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:${CUSTOM_CXX_FLAGS}>
  $<$<COMPILE_LANGUAGE:C>:${CUSTOM_C_FLAGS}>)

# Tests may also access private component headers
TARGET_INCLUDE_DIRECTORIES(${GEST_BINARY_NAME}
  PRIVATE ${CMAKE_SOURCE_DIR}/REST
  )

# Linkage with gtest_main coverage etc.
TARGET_LINK_LIBRARIES(
  ${GEST_BINARY_NAME}
  ${DUT_LIBS}   # Units under test
  GMock # main not required, implemented in test.cpp
  GTest
  Qt5::Test
  OpenSSL::Crypto
  OpenSSL::SSL
  ${CUSTOM_LINK_FLAGS}
  )


#------------------------------
# Rest Server binary
#------------------------------
add_executable(${RESTSERVER_NAME}
   ${CMAKE_CURRENT_SOURCE_DIR}/restserver.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/testcommon.cpp
  )
  
TARGET_COMPILE_DEFINITIONS(${RESTSERVER_NAME}
  PUBLIC ${CPP_DEFS} TEST_LOG_NAME=${RESTSERVER_NAME}.log
  )
# Link all libraries 
TARGET_LINK_LIBRARIES(${RESTSERVER_NAME}
  ${DUT_LIBS}   # Units under test
  OpenSSL::Crypto
  OpenSSL::SSL
  ${CUSTOM_LINK_FLAGS}
  )


IF(TEST_COVERAGE)
  IF(NOT MSVC)
	#Remove googletest, autogenerated QT code and headers for testcoverage
	SET(COVERAGE_EXCLUDES
	   '*GTestExternal/*'
	   '*PistacheExternal/*'
	   "*_autogen*"
	   '/usr/*'
	   )
	   
	SETUP_TARGET_FOR_COVERAGE(
	  NAME ${GEST_BINARY_NAME}_coverage  # New target name
	  EXECUTABLE ${GEST_BINARY_NAME}     # Executable in PROJECT_BINARY_DIR
	  DEPENDENCIES ${GEST_BINARY_NAME}
	  )
  ELSE(NOT MSVC)
	SET_PROPERTY(TARGET ${GEST_BINARY_NAME} 
		APPEND PROPERTY LINK_FLAGS  /PROFILE)
	SET_PROPERTY(TARGET ${RESTSERVER_NAME} 
		APPEND PROPERTY LINK_FLAGS  /PROFILE)			
  ENDIF(NOT MSVC)
ENDIF(TEST_COVERAGE)


ADD_TEST(restapi
	"${CMAKE_SOURCE_DIR}/test/resttest.py"
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)
set_property(TEST restapi 
	APPEND PROPERTY
    ENVIRONMENT PYTHONPATH=${CMAKE_SOURCE_DIR}/test/python-api)	
set_property(TEST restapi 
	APPEND PROPERTY
    ENVIRONMENT PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}:$ENV{PATH})	



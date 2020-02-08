MESSAGE(STATUS "Checking ${CMAKE_CURRENT_SOURCE_DIR} ")

# name of library (without lib- prefix)
set(LIBRARY_NAME REST)
# Component name (what to build and install) used in add_executable or add_library
set(COMPONENT_NAME ${LIBRARY_NAME})
# Interface/binary version
SET(COMPONENT_VERSION ${PROJECT_VERSION})
#includes etc. in folder
SET(COMPONENT_PATH ${LIBRARY_NAME})

# Check if Pistache is installed
find_package(Pistache)

# QT5 Components used in library
find_package(Qt5 COMPONENTS Core  Network REQUIRED)

# Threading library for gtest
# Use ${CMAKE_THREAD_LIBS_INIT} for the library
find_package(Threads REQUIRED)

#------------------------------
# add compile definitions
#------------------------------
SET(CPP_DEFS "")
LIST(APPEND CPP_DEFS "")

#-------------------------------------------------------------------------------
# Build from source if system does not provide pistache
IF(NOT PISTACHE_found)
   include(ExternalProject)
   set(PISTACHE_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/PistacheExternal/")
  
  # Download and unpack googletest at configure time
  configure_file(CMakeLists.txt.in ${PISTACHE_PREFIX}/CMakeLists.txt)

  message(STATUS "Executing cmake on generated Download CMakeLists.txt") 
  execute_process(COMMAND ${CMAKE_COMMAND} 
        -G "${CMAKE_GENERATOR}"  .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${PISTACHE_PREFIX} )
  if(result)
  	message(FATAL_ERROR "CMake step for Pistache failed: ${result}")
  endif()

  message(STATUS "Building Pistache" ) 
  execute_process(COMMAND ${CMAKE_COMMAND} --build . --parallel
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${PISTACHE_PREFIX})
  if(result)
	message(FATAL_ERROR "Build step for pistache failed: ${result}")
  endif()


  set(LIBPREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}")
  set(LIBSUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")
  set(PISTACHE_INCLUDES "${PISTACHE_PREFIX}/include")
  set(PISTACHE_LIBRARY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${LIBPREFIX}pistache")

  # Add Pistache directly to our build
  add_subdirectory(
    ${PISTACHE_PREFIX}/pistache-src 
    ${PISTACHE_PREFIX}/build 
    EXCLUDE_FROM_ALL)
    
  add_library(Pistache IMPORTED SHARED GLOBAL)
  set_target_properties(Pistache PROPERTIES
 	IMPORTED_LOCATION              "${LIBPREFIX}pistache"
 	INTERFACE_INCLUDE_DIRECTORIES  "${PISTACHE_INCLUDES}"
  	IMPORTED_LINK_INTERFACE_LIBRARIES  
  	Pistache
  	)
  	
  add_dependencies(Pistache pistache)
endif(NOT PISTACHE_found)

#-------------------------------------------------------------------------------
# Input sources
#-------------------------------------------------------------------------------
SET(SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/RestAdapter.cpp
  ) 

#------------------------------
# Output a library
#------------------------------ 
ADD_LIBRARY(${COMPONENT_NAME} SHARED
    ${SRCS} 
    )

SET_TARGET_PROPERTIES(
  ${COMPONENT_NAME} PROPERTIES
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
  PUBLIC ${CPP_DEFS} 
  )

TARGET_COMPILE_OPTIONS(${COMPONENT_NAME} 
  PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:${CUSTOM_CXX_FLAGS}>
  $<$<COMPILE_LANGUAGE:C>:${CUSTOM_C_FLAGS}>
  )

TARGET_LINK_LIBRARIES(${COMPONENT_NAME}
  	PUBLIC
 	Qt5::Core
 	Qt5::Network
 	${CMAKE_THREAD_LIBS_INIT}
    PRIVATE
    pistache
    ${CMAKE_THREAD_LIBS_INIT}
    )
#-----
# Install
#-----
include(InstallLibraryComponents)


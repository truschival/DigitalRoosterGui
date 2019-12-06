MESSAGE(STATUS 
    "** Generating Package Configurations for ${COMPONENT_NAME} **")

SET(version_config "${GENERATED_DIR}/${COMPONENT_NAME}ConfigVersion.cmake")
SET(component_config "${GENERATED_DIR}/${COMPONENT_NAME}Config.cmake")
SET(targets_export_name "${COMPONENT_NAME}Targets")

include(CMakePackageConfigHelpers)
WRITE_BASIC_PACKAGE_VERSION_FILE(
  ${version_config}
  VERSION ${COMPONENT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Note: variable 'targets_export_name' used
CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/cmake/Config.cmake.in"
  "${component_config}" @ONLY)

INSTALL(TARGETS ${COMPONENT_NAME}
  EXPORT ${targets_export_name}
  COMPONENT DEVELOP
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  RUNTIME DESTINATION ${INSTALL_BIN_DIR}
  # this will add -I<prefix>include/component to client compile flags
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
  NAMESPACE "${PROJECT_NAME}::"
  DESTINATION "${INSTALL_CMAKE_DIR}/${COMPONENT_NAME}"
  )

INSTALL(
  FILES "${component_config}" "${version_config}"
  COMPONENT DEVELOP
  DESTINATION "${INSTALL_CMAKE_DIR}/${PROJECT_NAME}/${COMPONENT_NAME}"
  )
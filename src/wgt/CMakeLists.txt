# Target - sources
SET(SRCS
  rds_parser.cc
  step/step_check_settings_level.cc
  step/step_create_symbolic_link.cc
  step/step_encrypt_resources.cc
  step/step_generate_xml.cc
  step/step_parse.cc
  step/step_remove_encryption_data.cc
  step/step_parse_recovery.cc
  step/step_rds_parse.cc
  step/step_rds_modify.cc
  step/step_wgt_create_icons.cc
  step/step_wgt_create_storage_directories.cc
  step/step_wgt_copy_storage_directories.cc
  step/step_wgt_resource_directory.cc
  step/step_add_default_privileges.cc
  wgt_app_query_interface.cc
  wgt_installer.cc
)

IF(WRT_LAUNCHER)
    ADD_DEFINITIONS("-DWRT_LAUNCHER=\"${WRT_LAUNCHER}\"")
    MESSAGE( "WRT LAUNCHER binary path is  set to ${WRT_LAUNCHER}")
ELSE(WRT_LAUNCHER)
    MESSAGE(FATAL_ERROR, "WRT LAUNCHER binary path is not set")
ENDIF(WRT_LAUNCHER)

# Target - definition
ADD_LIBRARY(${TARGET_LIBNAME_WGT} STATIC ${SRCS})
ADD_EXECUTABLE(${TARGET_WGT_BACKEND} "wgt_backend.cc")
# Target - includes
TARGET_INCLUDE_DIRECTORIES(${TARGET_LIBNAME_WGT} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")
TARGET_INCLUDE_DIRECTORIES(${TARGET_WGT_BACKEND} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")
# Target - deps
APPLY_PKG_CONFIG(${TARGET_LIBNAME_WGT} PUBLIC
  MANIFEST_HANDLERS_DEPS
  MANIFEST_PARSER_DEPS
  PKGMGR_INSTALLER_DEPS
  ENCRYPTION_DEPS
)

# Target - in-package deps
TARGET_LINK_LIBRARIES(${TARGET_LIBNAME_WGT} PUBLIC ${TARGET_LIBNAME_COMMON})
TARGET_LINK_LIBRARIES(${TARGET_WGT_BACKEND} PRIVATE ${TARGET_LIBNAME_WGT})

# Install
INSTALL(TARGETS ${TARGET_LIBNAME_WGT} DESTINATION ${LIB_INSTALL_DIR})
INSTALL(TARGETS ${TARGET_WGT_BACKEND} DESTINATION ${BINDIR})

# Target - sources
SET(SRCS
  privileges.cc
  app_installer.cc
  backup_paths.cc
  certificate_validation.cc
  external_storage.cc
  feature_validator.cc
  installer_context.cc
  pkgdir_tool_request.cc
  plugins/plugin_factory.cc
  plugins/plugin_manager.cc
  plugins/plugin_list_parser.cc
  plugins/plugin_xml_parser.cc
  plugins/plugin_factory.cc
  plugins/plugin.cc
  plugins/types/category_plugin.cc
  plugins/types/metadata_plugin.cc
  plugins/types/tag_plugin.cc
  pkgmgr_interface.cc
  pkgmgr_registration.cc
  pkgmgr_signal.cc
  rds_parser.cc
  recovery_file.cc
  request.cc
  security_registration.cc
  shared_dirs.cc
  step/backup/step_backup_icons.cc
  step/backup/step_backup_manifest.cc
  step/backup/step_copy_backup.cc
  step/configuration/step_block_cross_update.cc
  step/configuration/step_check_tizen_version.cc
  step/configuration/step_configure.cc
  step/configuration/step_fail.cc
  step/configuration/step_parse_manifest.cc
  step/filesystem/step_acquire_external_storage.cc
  step/filesystem/step_clear_data.cc
  step/filesystem/step_copy.cc
  step/filesystem/step_copy_storage_directories.cc
  step/filesystem/step_copy_tep.cc
  step/filesystem/step_create_icons.cc
  step/filesystem/step_create_per_user_storage_directories.cc
  step/filesystem/step_create_storage_directories.cc
  step/filesystem/step_delta_patch.cc
  step/filesystem/step_move_installed_storage.cc
  step/filesystem/step_recover_files.cc
  step/filesystem/step_recover_icons.cc
  step/filesystem/step_recover_manifest.cc
  step/filesystem/step_recover_external_storage.cc
  step/filesystem/step_recover_storage_directories.cc
  step/filesystem/step_remove_files.cc
  step/filesystem/step_remove_icons.cc
  step/filesystem/step_remove_per_user_storage_directories.cc
  step/filesystem/step_remove_temporary_directory.cc
  step/filesystem/step_remove_zip_image.cc
  step/filesystem/step_unzip.cc
  step/mount/step_mount_install.cc
  step/mount/step_mount_unpacked.cc
  step/mount/step_mount_update.cc
  step/pkgmgr/step_check_removable.cc
  step/pkgmgr/step_check_restriction.cc
  step/pkgmgr/step_kill_apps.cc
  step/pkgmgr/step_recover_application.cc
  step/pkgmgr/step_register_app.cc
  step/pkgmgr/step_remove_manifest.cc
  step/pkgmgr/step_run_parser_plugins.cc
  step/pkgmgr/step_unregister_app.cc
  step/pkgmgr/step_update_app.cc
  step/pkgmgr/step_update_tep.cc
  step/rds/step_rds_modify.cc
  step/rds/step_rds_parse.cc
  step/recovery/step_open_recovery_file.cc
  step/recovery/step_recovery.cc
  step/security/step_check_background_category.cc
  step/security/step_check_old_certificate.cc
  step/security/step_check_signature.cc
  step/security/step_privilege_compatibility.cc
  step/security/step_recover_security.cc
  step/security/step_register_security.cc
  step/security/step_revoke_security.cc
  step/security/step_rollback_deinstallation_security.cc
  step/security/step_rollback_installation_security.cc
  step/security/step_update_security.cc
  tzip_interface.cc
  utils/base64.cc
  utils/file_util.cc
  utils/subprocess.cc
)
# Target - definition
ADD_LIBRARY(${TARGET_LIBNAME_COMMON} SHARED ${SRCS})
# Target - includes
TARGET_INCLUDE_DIRECTORIES(${TARGET_LIBNAME_COMMON} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")
# Target - deps
APPLY_PKG_CONFIG(${TARGET_LIBNAME_COMMON} PUBLIC
  AUL_DEPS
  PKGMGR_DEPS
  PKGMGR_INFO_DEPS
  PKGMGR_PARSER_DEPS
  PKGMGR_INSTALLER_DEPS
  MANIFEST_PARSER_UTILS_DEPS
  SECURITY_MANAGER_DEPS
  TZPLATFORM_CONFIG_DEPS
  LIBXML_DEPS
  CERT_SVC_DEPS_VCORE_DEPS
  MINIZIP_DEPS
  ZLIB_DEPS
  PRIVILEGE_CHECKER_DEPS
  DELTA_MANIFEST_HANDLERS_DEPS
  TPK_MANIFEST_HANDLERS_DEPS
  GDBUS_DEPS
  GUM_DEPS
  APP2SD_DEPS
  CAPI_SYSTEM_INFO_DEPS
  Boost
)

# Extra
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_COMMON} PROPERTIES VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_COMMON} PROPERTIES SOVERSION ${VERSION_MAJOR})
TARGET_LINK_LIBRARIES(${TARGET_LIBNAME_COMMON} PRIVATE "-lattr")

# Install
INSTALL(TARGETS ${TARGET_LIBNAME_COMMON} DESTINATION ${LIB_INSTALL_DIR})


# Plugins
SET(PLUGINS_LIST_FILE_NAME plugins_list.txt)
SET(PLUGINS_LIST_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/plugins/${PLUGINS_LIST_FILE_NAME})

SET(PLUGINS_LIST_INSTALL_PATH ${SHAREDIR}/app-installers)
SET(PLUGINS_LIST_INSTALL_FILE_PATH ${PLUGINS_LIST_INSTALL_PATH}/${PLUGINS_LIST_FILE_NAME})

ADD_DEFINITIONS("-DPLUGINS_LIST_INSTALL_FILE_PATH=\"${PLUGINS_LIST_INSTALL_FILE_PATH}\"")
IF(NOT DEFINED TIZEN_FULL_VERSION)
  MESSAGE(FATAL_ERROR "TIZEN_FULL_VERSION MUST BE defined")
ELSE(NOT DEFINED TIZEN_FULL_VERSION)
  ADD_DEFINITIONS("-DTIZEN_FULL_VERSION=\"${TIZEN_FULL_VERSION}\"")
ENDIF(NOT DEFINED TIZEN_FULL_VERSION)
CONFIGURE_FILE(${PLUGINS_LIST_FILE_PATH}.in ${PLUGINS_LIST_FILE_PATH} @ONLY)
INSTALL(FILES ${PLUGINS_LIST_FILE_PATH} DESTINATION ${PLUGINS_LIST_INSTALL_PATH}/)

# Export devel package
CONFIGURE_FILE(app-installers.pc.in app-installers.pc @ONLY)
INSTALL(FILES app-installers.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
INSTALL(DIRECTORY ./ DESTINATION ${INCLUDEDIR}/app-installers/common/
        FILES_MATCHING PATTERN "*.h")

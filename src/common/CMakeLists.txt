# Target - sources
SET(SRCS
  app_installer.cc
  backup_paths.cc
  installer_context.cc
  package_storage.cc
  package_external_storage.cc
  package_internal_storage.cc
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
  recovery_file.cc
  request.cc
  security_registration.cc
  step/step_acquire_package_storage.cc
  step/step_backup_icons.cc
  step/step_backup_manifest.cc
  step/step_unzip.cc
  step/step_create_icons.cc
  step/step_check_background_category.cc
  step/step_check_blacklist.cc
  step/step_check_old_certificate.cc
  step/step_check_signature.cc
  step/step_clear_data.cc
  step/step_configure.cc
  step/step_copy.cc
  step/step_copy_tep.cc
  step/step_copy_backup.cc
  step/step_copy_storage_directories.cc
  step/step_create_storage_directories.cc
  step/step_create_per_user_storage_directories.cc
  step/step_delta_patch.cc
  step/step_fail.cc
  step/step_kill_apps.cc
  step/step_parse_manifest.cc
  step/step_recover_application.cc
  step/step_recover_files.cc
  step/step_recover_icons.cc
  step/step_recover_manifest.cc
  step/step_recover_package_storage.cc
  step/step_recover_security.cc
  step/step_recover_storage_directories.cc
  step/step_recovery.cc
  step/step_register_app.cc
  step/step_open_recovery_file.cc
  step/step_privilege_compatibility.cc
  step/step_remove_icons.cc
  step/step_remove_files.cc
  step/step_remove_per_user_storage_directories.cc
  step/step_remove_temporary_directory.cc
  step/step_revoke_security.cc
  step/step_register_security.cc
  step/step_rollback_deinstallation_security.cc
  step/step_rollback_installation_security.cc
  step/step_run_parser_plugins.cc
  step/step_unregister_app.cc
  step/step_update_app.cc
  step/step_update_security.cc
  step/step_update_tep.cc
  step/step_remove_manifest.cc
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
  APPMANAGER_DEPS
  DELTA_MANIFEST_HANDLERS_DEPS
  TPK_MANIFEST_HANDLERS_DEPS
  APP2SD_DEPS
  Boost
)

# Extra
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_COMMON} PROPERTIES VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_COMMON} PROPERTIES SOVERSION ${VERSION_MAJOR})

# Install
INSTALL(TARGETS ${TARGET_LIBNAME_COMMON} DESTINATION ${LIB_INSTALL_DIR})


# Plugins
SET(PLUGINS_LIST_FILE_NAME plugins_list.txt)
SET(PLUGINS_LIST_FILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/plugins/${PLUGINS_LIST_FILE_NAME})

SET(PLUGINS_LIST_INSTALL_PATH ${SHAREDIR}/app-installers/)
SET(PLUGINS_LIST_INSTALL_FILE_PATH ${PLUGINS_LIST_INSTALL_PATH}/${PLUGINS_LIST_FILE_NAME})

ADD_DEFINITIONS("-DPLUGINS_LIST_INSTALL_FILE_PATH=\"${PLUGINS_LIST_INSTALL_FILE_PATH}\"")
CONFIGURE_FILE(${PLUGINS_LIST_FILE_PATH}.in ${PLUGINS_LIST_FILE_PATH} @ONLY)
INSTALL(FILES ${PLUGINS_LIST_FILE_PATH} DESTINATION ${PLUGINS_LIST_INSTALL_PATH}/)

# Export devel package
CONFIGURE_FILE(app-installers.pc.in app-installers.pc @ONLY)
INSTALL(FILES app-installers.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
INSTALL(DIRECTORY ./ DESTINATION ${INCLUDEDIR}/app-installers/common/
        FILES_MATCHING PATTERN "*.h")

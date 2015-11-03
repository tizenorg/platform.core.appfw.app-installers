# Target - sources
SET(SRCS
  app_installer.cc
  backup_paths.cc
  installer_context.cc
  pkgmgr_interface.cc
  pkgmgr_registration.cc
  pkgmgr_signal.cc
  recovery_file.cc
  request.cc
  security_registration.cc
  step/step_backup_icons.cc
  step/step_backup_manifest.cc
  step/step_unzip.cc
  step/step_create_icons.cc
  step/step_check_old_certificate.cc
  step/step_check_signature.cc
  step/step_configure.cc
  step/step_copy.cc
  step/step_copy_backup.cc
  step/step_copy_storage_directories.cc
  step/step_create_storage_directories.cc
  step/step_fail.cc
  step/step_kill_apps.cc
  step/step_generate_xml.cc
  step/step_recover_application.cc
  step/step_recover_files.cc
  step/step_recover_icons.cc
  step/step_recover_manifest.cc
  step/step_recover_security.cc
  step/step_recover_storage_directories.cc
  step/step_recovery.cc
  step/step_register_app.cc
  step/step_old_manifest.cc
  step/step_open_recovery_file.cc
  step/step_parse.cc
  step/step_privilege_compatibility.cc
  step/step_remove_icons.cc
  step/step_remove_files.cc
  step/step_remove_temporary_directory.cc
  step/step_revoke_security.cc
  step/step_register_security.cc
  step/step_rollback_deinstallation_security.cc
  step/step_rollback_installation_security.cc
  step/step_unregister_app.cc
  step/step_update_app.cc
  step/step_update_security.cc
  utils/file_util.cc
  utils/string_util.cc
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
  SECURITY_MANAGER_DEPS
  TZPLATFORM_CONFIG_DEPS
  LIBXML_DEPS
  CERT_SVC_DEPS_VCORE
  MINIZIP_DEPS
  ZLIB_DEPS
  PRIVILEGE_CHECKER_DEPS
  APPMANAGER_DEPS
  Boost
)

IF(_APPFW_FEATURE_EXPANSION_PKG_INSTALL)
  ADD_DEFINITIONS("-D_APPFW_FEATURE_EXPANSION_PKG_INSTALL")
ENDIF(_APPFW_FEATURE_EXPANSION_PKG_INSTALL)

# Extra
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_COMMON} PROPERTIES VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_COMMON} PROPERTIES SOVERSION ${VERSION_MAJOR})

# Install
INSTALL(TARGETS ${TARGET_LIBNAME_COMMON} DESTINATION ${LIB_INSTALL_DIR})

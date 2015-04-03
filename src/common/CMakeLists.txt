# Target - sources
SET(SRCS
  app_installer.cc
  context_installer.cc
  pkgmgr_interface.cc
  pkgmgr_signal.cc
  security_registration.cc
  step/step_unzip.cc
  step/step_check_signature.cc
  step/step_copy.cc
  step/step_generate_xml.cc
  step/step_register_app.cc
  step/step_parse.cc
  step/step_remove_files.cc
  step/step_revoke_security.cc
  step/step_register_security.cc
  step/step_unregister_app.cc
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
  MINIZIP_DEPS
  ZLIB_DEPS
  TZPLATFORM_CONFIG_DEPS
  LIBXML_DEPS
  Boost
)
# Target in-package deps
TARGET_LINK_LIBRARIES(${TARGET_LIBNAME_COMMON} PUBLIC ${TARGET_LIBNAME_SIGNATURE})

# Extra
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_COMMON} PROPERTIES VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_COMMON} PROPERTIES SOVERSION ${VERSION_MAJOR})

# Install
INSTALL(TARGETS ${TARGET_LIBNAME_COMMON} DESTINATION ${LIB_INSTALL_DIR})

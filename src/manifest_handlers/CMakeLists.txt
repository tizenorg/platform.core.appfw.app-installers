# Target - sources
SET(SRCS
  app_control_handler.cc
  application_manifest_constants.cc
  application_icons_handler.cc
  appwidget_handler.cc
  category_handler.cc
  ime_handler.cc
  metadata_handler.cc
  navigation_handler.cc
  permissions_handler.cc
  setting_handler.cc
  service_handler.cc
  splash_screen_handler.cc
  tizen_application_handler.cc
  widget_handler.cc
)

# Target - definition
ADD_LIBRARY(${TARGET_LIBNAME_MANIFEST_HANDLERS} SHARED ${SRCS})
# Target - includes
TARGET_INCLUDE_DIRECTORIES(${TARGET_LIBNAME_MANIFEST_HANDLERS} PUBLIC
                           "${CMAKE_CURRENT_SOURCE_DIR}/../")

# Extra
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_MANIFEST_HANDLERS} PROPERTIES VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_MANIFEST_HANDLERS} PROPERTIES SOVERSION ${VERSION_MAJOR})

# Install
INSTALL(TARGETS ${TARGET_LIBNAME_MANIFEST_HANDLERS} DESTINATION ${LIB_INSTALL_DIR})

# Install includes
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h")

# Configure and install pkgconfig file
SET(PKG_FILE manifest-handlers.pc)
CONFIGURE_FILE(${PKG_FILE}.in ${PKG_FILE} @ONLY)
INSTALL(FILES ${PKG_FILE} DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

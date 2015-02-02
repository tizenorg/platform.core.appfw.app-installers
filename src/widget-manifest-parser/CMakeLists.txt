# Target - sources
SET(SRCS
  application_data.cc
  application_manifest_constants.cc
  manifest.cc
  manifest_handler.cc
  manifest_handlers/category_handler.cc
  manifest_handlers/ime_handler.cc
  manifest_handlers/permissions_handler.cc
  manifest_handlers/setting_handler.cc
  manifest_handlers/tizen_application_handler.cc
  manifest_handlers/widget_handler.cc
  manifest_util.cc
  widget_manifest_parser.cc
)
# Target - definition
ADD_LIBRARY(${TARGET_LIBNAME_WIDGET_MANIFEST_PARSER} SHARED ${SRCS})
# Target - includes
TARGET_INCLUDE_DIRECTORIES(${TARGET_LIBNAME_WIDGET_MANIFEST_PARSER} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")
# Target - deps
APPLY_PKG_CONFIG(${TARGET_LIBNAME_WIDGET_MANIFEST_PARSER} PUBLIC
  LIBXML_DEPS
  Boost
)

TARGET_LINK_LIBRARIES(${TARGET_LIBNAME_WIDGET_MANIFEST_PARSER} PUBLIC ${TARGET_LIBNAME_UTILS})

# Extra
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_WIDGET_MANIFEST_PARSER} PROPERTIES VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_LIBNAME_WIDGET_MANIFEST_PARSER} PROPERTIES SOVERSION ${VERSION_MAJOR})

# Install
INSTALL(TARGETS ${TARGET_LIBNAME_WIDGET_MANIFEST_PARSER} DESTINATION ${LIB_INSTALL_DIR})

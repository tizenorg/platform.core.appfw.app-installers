SET(PRJ_PATH ..)

SET(TESTS
  values_unittest
  manifest_unittest
  manifest_handler_unittest
  manifest_util_unittest
  string_util_unittest
  widget_manifest_parser_unittest
  xml_parser_unittest
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../)

# Executables
ADD_EXECUTABLE(values_unittest
               utils_values_unittest.cc)
ADD_EXECUTABLE(manifest_unittest
               widget_manifest_parser_manifest_unittest.cc)
ADD_EXECUTABLE(manifest_handler_unittest
               widget_manifest_parser_manifest_handler_unittest.cc)
ADD_EXECUTABLE(manifest_util_unittest
               widget_manifest_parser_manifest_util_unittest.cc)
ADD_EXECUTABLE(string_util_unittest string_util_unittest.cc)
ADD_EXECUTABLE(widget_manifest_parser_unittest
               widget_manifest_parser_unittest.cc)
ADD_EXECUTABLE(xml_parser_unittest
               xml_parser_unittest.cc ../xml_parser/xml_parser.cc)

INSTALL(DIRECTORY test_samples/ DESTINATION ${SHAREDIR}/app-installers-ut/test_samples)

FOREACH(test ${TESTS})
APPLY_PKG_CONFIG(${test} PUBLIC
  LIBXML_DEPS
  PKGMGR_INFO_DEPS
  PKGMGR_PARSER_DEPS
  VCONF_DEPS
  Boost
  GTEST
)
ENDFOREACH(test)

# FindGTest module do not sets all needed libraries in GTEST_LIBRARIES and
# GTest main libraries is still missing, so additional linking of
# GTEST_MAIN_LIBRARIES is needed.
target_link_libraries(values_unittest PUBLIC ${TARGET_LIBNAME_UTILS} ${GTEST_MAIN_LIBRARIES})
target_link_libraries(manifest_unittest PUBLIC ${TARGET_LIBNAME_WIDGET_MANIFEST_PARSER} ${GTEST_MAIN_LIBRARIES})
target_link_libraries(manifest_handler_unittest PUBLIC ${TARGET_LIBNAME_WIDGET_MANIFEST_PARSER} ${GTEST_MAIN_LIBRARIES})
target_link_libraries(manifest_util_unittest PUBLIC ${TARGET_LIBNAME_WIDGET_MANIFEST_PARSER} ${GTEST_MAIN_LIBRARIES})
target_link_libraries(string_util_unittest PUBLIC ${TARGET_LIBNAME_UTILS} ${GTEST_MAIN_LIBRARIES})
target_link_libraries(widget_manifest_parser_unittest PUBLIC ${TARGET_LIBNAME_WIDGET_MANIFEST_PARSER} ${GTEST_MAIN_LIBRARIES})
target_link_libraries(xml_parser_unittest PUBLIC ${TARGET_LIBNAME_XML_PARSER} ${GTEST_MAIN_LIBRARIES})

FOREACH(test ${TESTS})
INSTALL(TARGETS ${test} DESTINATION ${BINDIR}/app-installers-ut)
ENDFOREACH(test)

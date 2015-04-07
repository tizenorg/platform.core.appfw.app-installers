SET(PRJ_PATH ..)

IF(BUILD_PARSER)
  SET(TESTS
    values_unittest
    manifest_unittest
    manifest_handler_unittest
    manifest_util_unittest
    parser_unittest
    service_handler_unittest
    string_util_unittest
  )
  SET(DESTINATION_DIR manifest-parser-ut)
ELSE(BUILD_PARSER)
  SET(TESTS
    signature_unittest
    xml_parser_unittest
  )
  SET(DESTINATION_DIR app-installers-ut)
ENDIF(BUILD_PARSER)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../)

# Executables
IF(BUILD_PARSER)
  ADD_EXECUTABLE(values_unittest
                 utils_values_unittest.cc)
  ADD_EXECUTABLE(manifest_unittest
                 parser_manifest_unittest.cc)
  ADD_EXECUTABLE(manifest_handler_unittest
                 parser_manifest_handler_unittest.cc)
  ADD_EXECUTABLE(manifest_util_unittest
                 parser_manifest_util_unittest.cc)
  ADD_EXECUTABLE(service_handler_unittest
                 service_handler_unittest.cc)
  ADD_EXECUTABLE(parser_unittest
                 parser_unittest.cc)
  ADD_EXECUTABLE(string_util_unittest string_util_unittest.cc)
ELSE(BUILD_PARSER)
  ADD_EXECUTABLE(signature_unittest
                 signature_unittest.cc)
  ADD_EXECUTABLE(xml_parser_unittest
                 xml_parser_unittest.cc ../xml_parser/xml_parser.cc)
ENDIF(BUILD_PARSER)

INSTALL(DIRECTORY test_samples/ DESTINATION ${SHAREDIR}/${DESTINATION_DIR}/test_samples)

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
IF(BUILD_PARSER)
  target_link_libraries(values_unittest PUBLIC ${TARGET_LIBNAME_PARSER} ${GTEST_MAIN_LIBRARIES})
  target_link_libraries(manifest_unittest PUBLIC ${TARGET_LIBNAME_PARSER} ${GTEST_MAIN_LIBRARIES})
  target_link_libraries(manifest_handler_unittest PUBLIC ${TARGET_LIBNAME_PARSER} ${GTEST_MAIN_LIBRARIES})
  target_link_libraries(manifest_util_unittest PUBLIC ${TARGET_LIBNAME_PARSER} ${TARGET_LIBNAME_MANIFEST_HANDLERS} ${GTEST_MAIN_LIBRARIES})
  target_link_libraries(parser_unittest PUBLIC ${TARGET_LIBNAME_PARSER} ${GTEST_MAIN_LIBRARIES})
  target_link_libraries(service_handler_unittest PUBLIC ${TARGET_LIBNAME_PARSER} ${TARGET_LIBNAME_MANIFEST_HANDLERS} ${GTEST_MAIN_LIBRARIES})
  target_link_libraries(string_util_unittest PUBLIC ${TARGET_LIBNAME_PARSER} ${GTEST_MAIN_LIBRARIES})
ELSE(BUILD_PARSER)
  target_link_libraries(signature_unittest PUBLIC ${TARGET_LIBNAME_SIGNATURE} ${GTEST_MAIN_LIBRARIES})
  target_link_libraries(xml_parser_unittest PUBLIC ${TARGET_LIBNAME_PARSER} ${GTEST_MAIN_LIBRARIES})
ENDIF(BUILD_PARSER)

FOREACH(test ${TESTS})
  INSTALL(TARGETS ${test} DESTINATION ${BINDIR}/${DESTINATION_DIR})
ENDFOREACH(test)

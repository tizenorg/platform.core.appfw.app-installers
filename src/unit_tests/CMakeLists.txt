SET(TESTS
  signature_unittest
  xml_parser_unittest
  smoke_test
)
SET(DESTINATION_DIR app-installers-ut)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../)

# Executables
ADD_EXECUTABLE(signature_unittest
  signature_unittest.cc
)
ADD_EXECUTABLE(xml_parser_unittest
  xml_parser_unittest.cc
  ../tpk/xml_parser/xml_parser.cc
)
ADD_EXECUTABLE(smoke_test
  smoke_test.cc
)

INSTALL(DIRECTORY test_samples/ DESTINATION ${SHAREDIR}/${DESTINATION_DIR}/test_samples)
INSTALL(FILES run-unit-tests.sh DESTINATION ${BINDIR}/app-installers-ut/
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ)

FOREACH(test ${TESTS})
APPLY_PKG_CONFIG(${test} PUBLIC
  LIBXML_DEPS
  Boost
  GTEST
)
ENDFOREACH(test)

# FindGTest module do not sets all needed libraries in GTEST_LIBRARIES and
# GTest main libraries is still missing, so additional linking of
# GTEST_MAIN_LIBRARIES is needed.
target_link_libraries(signature_unittest PUBLIC ${TARGET_LIBNAME_COMMON} ${GTEST_MAIN_LIBRARIES})
target_link_libraries(xml_parser_unittest PUBLIC ${TARGET_LIBNAME_PARSER} ${GTEST_MAIN_LIBRARIES})
target_link_libraries(smoke_test PRIVATE ${TARGET_LIBNAME_WGT} ${GTEST_MAIN_LIBRARIES})

FOREACH(test ${TESTS})
  INSTALL(TARGETS ${test} DESTINATION ${BINDIR}/${DESTINATION_DIR})
ENDFOREACH(test)

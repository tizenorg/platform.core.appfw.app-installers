SET(DESTINATION_DIR app-installers-ut)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../)

# Executables
ADD_EXECUTABLE(signature_unittest
  signature_unittest.cc
)

INSTALL(DIRECTORY test_samples/ DESTINATION ${SHAREDIR}/${DESTINATION_DIR}/test_samples)

APPLY_PKG_CONFIG(signature_unittest PUBLIC
  Boost
  GTEST
)

# FindGTest module do not sets all needed libraries in GTEST_LIBRARIES and
# GTest main libraries is still missing, so additional linking of
# GTEST_MAIN_LIBRARIES is needed.
TARGET_LINK_LIBRARIES(signature_unittest PUBLIC ${TARGET_LIBNAME_COMMON} ${GTEST_MAIN_LIBRARIES} pthread)

INSTALL(TARGETS signature_unittest DESTINATION ${BINDIR}/${DESTINATION_DIR})

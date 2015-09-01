add_executable(pkgdir_maker "pkgdir_maker.cc")

INSTALL(FILES
		${CMAKE_CURRENT_SOURCE_DIR}/pkgdir_maker_impl.sh
		${CMAKE_CURRENT_BINARY_DIR}/pkgdir_maker
        DESTINATION ${BINDIR})


# Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

#
# This function applies external (out of source tree) dependencies
# to given target. Arguments are:
#   TARGET - valid cmake target
#   PRIVACY - dependency can be inherited by dependent targets or not:
#     PUBLIC - this should be used by default, cause compile/link flags passing
#     PRIVATE - do not passes any settings to dependent targets,
#               may be usefull for static libraries from the inside of the project
# Argument ARGV2 and following are supposed to be names of checked pkg config
# packages. This function will use variables created by check_pkg_modules().
#  - ${DEP_NAME}_LIBRARIES
#  - ${DEP_NAME}_INCLUDE_DIRS
#  - ${DEP_NAME}_CFLAGS
#
FUNCTION(APPLY_PKG_CONFIG TARGET PRIVACY)
  MATH(EXPR DEST_INDEX "${ARGC}-1")
  FOREACH(I RANGE 2 ${DEST_INDEX})
    TARGET_LINK_LIBRARIES(${TARGET} LINK_${PRIVACY} "${${ARGV${I}}_LIBRARIES}")
    TARGET_INCLUDE_DIRECTORIES(${TARGET} ${PRIVACY} "${${ARGV${I}}_INCLUDE_DIRS}")
    SET_TARGET_PROPERTIES(${TARGET} PROPERTIES SKIP_BUILD_RPATH true)
  ENDFOREACH(I RANGE 2 ${DEST_INDEX})
ENDFUNCTION(APPLY_PKG_CONFIG TARGET PRIVACY)

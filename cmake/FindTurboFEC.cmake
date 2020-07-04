# - Try to find turbofec
# Once done this will define
#
#  TURBOFEC_FOUND - system has turbofec
#  TURBOFEC_INCLUDE_DIRS - the turbofec include directory
#  TURBOFEC_LIBRARIES - Link these to use turbofec
#
# Redistribution and use is allowed according to the terms of the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

include(FindPkgConfig)
pkg_check_modules(PC_TURBOFEC QUIET turbofec)
set(TURBOFEC_DEFINITIONS ${PC_TURBOFEC_CFLAGS_OTHER})

find_path(TURBOFEC_INCLUDE_DIR turbofec/turbo.h
          HINTS ${PC_TURBOFEC_INCLUDEDIR} ${PC_TURBOFEC_INCLUDE_DIRS}
          PATH_SUFFIXES turbofec)

find_library(TURBOFEC_LIBRARY NAMES turbofec
             HINTS ${PC_TURBOFEC_LIBDIR} ${PC_TURBOFEC_LIBRARY_DIRS})

set(TURBOFEC_VERSION ${PC_TURBOFEC_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(turbofec
                                  REQUIRED_VARS TURBOFEC_LIBRARY TURBOFEC_INCLUDE_DIR
                                  VERSION_VAR TURBOFEC_VERSION)

mark_as_advanced(TURBOFEC_INCLUDE_DIR TURBOFEC_LIBRARY)

set(TURBOFEC_LIBRARIES ${TURBOFEC_LIBRARY})
set(TURBOFEC_INCLUDE_DIRS ${TURBOFEC_INCLUDE_DIR})


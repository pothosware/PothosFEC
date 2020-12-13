# - Try to find turbofec
# Once done this will define
#
#  LIBFEC_FOUND - system has turbofec
#  LIBFEC_INCLUDE_DIRS - the turbofec include directory
#  LIBFEC_LIBRARIES - Link these to use turbofec
#
# Redistribution and use is allowed according to the terms of the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

include(FindPkgConfig)
pkg_check_modules(PC_LIBFEC QUIET fec libfec)
set(LIBFEC_DEFINITIONS ${PC_LIBFEC_CFLAGS_OTHER})

find_path(LIBFEC_INCLUDE_DIR fec.h
          HINTS ${PC_LIBFEC_INCLUDEDIR} ${PC_LIBFEC_INCLUDE_DIRS}
          PATH_SUFFIXES fec libfec)

find_library(LIBFEC_LIBRARY NAMES fec
             HINTS ${PC_LIBFEC_LIBDIR} ${PC_LIBFEC_LIBRARY_DIRS})

set(LIBFEC_VERSION ${PC_LIBFEC_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libfec
                                  REQUIRED_VARS LIBFEC_LIBRARY LIBFEC_INCLUDE_DIR
                                  VERSION_VAR LIBFEC_VERSION)

mark_as_advanced(LIBFEC_INCLUDE_DIR LIBFEC_LIBRARY)

set(LIBFEC_LIBRARIES ${LIBFEC_LIBRARY})
set(LIBFEC_INCLUDE_DIRS ${LIBFEC_INCLUDE_DIR})


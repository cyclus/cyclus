# - Try to find SigC++-2.0
# Once done, this will define
#
#  SigC++_FOUND - system has SigC++
#  SigC++_INCLUDE_DIRS - the SigC++ include directories
#  SigC++_LIBRARIES - link these to use SigC++

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(SigC++_PKGCONF sigc++-2.0)

# Main include dir
find_path(SigC++_INCLUDE_DIR
    NAMES sigc++/sigc++.h
    ${DEPS_INCLUDE_HINTS}
    PATHS ${SigC++_PKGCONF_INCLUDE_DIRS} ${SigC++_PKGCONF_INCLUDE_DIRS}/include
    PATH_SUFFIXES include/sigc++-2.0 sigc++-2.0
    )

# Glib-related libraries also use a separate config header, which is in lib dir
find_path(SigC++Config_INCLUDE_DIR
    NAMES sigc++config.h
    ${DEPS_INCLUDE_HINTS}
    PATHS ${SigC++_PKGCONF_INCLUDE_DIRS} /usr
    PATH_SUFFIXES lib/sigc++-2.0/include
    )

# find lib
find_path(SigC++_PKGCONF_LIBRARY_DIRS
    NAMES libsigc-2.0.so
    ${DEPS_LIB_HINTS}
    PATH_SUFFIXES lib
    )
libfind_library(SigC++ sigc 2.0)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(SigC++_PROCESS_INCLUDES SigC++_INCLUDE_DIR SigC++Config_INCLUDE_DIR)
set(SigC++_PROCESS_LIBS SigC++_LIBRARY)
libfind_process(SigC++)
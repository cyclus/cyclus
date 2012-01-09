#!/bin/bash
##---------------------------------------------------------------------------##
## CONFIGURE TRILINOS ON BLACKBEARD WITH CMAKE
##---------------------------------------------------------------------------##

EXTRA_ARGS=$@

rm -rf CMakeCache.txt

##---------------------------------------------------------------------------##

#
# Note that the user must change /path/to/trilinos/source to the correct path
#

cmake \
    -D CMAKE_BUILD_TYPE:STRING=RELEASE \
    -D CMAKE_VERBOSE_MAKEFILE:BOOL=OFF \
    -D Trilinos_ENABLE_TESTS:BOOL=OFF \
    -D Trilinos_ENABLE_Teuchos:BOOL=ON \
    -D BUILD_SHARED_LIBS:BOOL=ON \
    -D CMAKE_INSTALL_PREFIX:PATH=/usr/local/ \
    $EXTRA_ARGS \
    /path/to/trilinos/source
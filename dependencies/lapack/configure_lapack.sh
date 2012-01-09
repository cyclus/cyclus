#!/bin/bash
##---------------------------------------------------------------------------##
## CONFIGURE LAPACK ON BLACKBEARD WITH CMAKE
##---------------------------------------------------------------------------##

EXTRA_ARGS=$@

rm -rf CMakeCache.txt

##---------------------------------------------------------------------------##

#
# Note that the user must change /path/to/lapack/source to the correct path
#

cmake \
    -D CMAKE_BUILD_TYPE:STRING=Release \
    -D BUILD_COMPLEX:BOOL=ON \
    -D BUILD_COMPLEX16:BOOL=ON \
    -D BUILD_DOUBLE:BOOL=ON \
    -D BUILD_SHARED_LIBS:BOOL=ON \
    -D BUILD_SINGLE:BOOL=ON \
    -D USE_OPTIMIZED_BLAS:BOOL=OFF \
    -D USE_XBLAS:BOOL=OFF \
    -D CMAKE_INSTALL_PREFIX:PATH=/usr/local \
    $EXTRA_ARGS \
    /path/to/lapack/source
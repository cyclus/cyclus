#!/bin/bash
##---------------------------------------------------------------------------##
## CONFIGURE TRILINOS ON BLACKBEARD WITH CMAKE
##---------------------------------------------------------------------------##

EXTRA_ARGS=$@

rm -rf CMakeCache.txt

##---------------------------------------------------------------------------##

cmake \
    -D CMAKE_BUILD_TYPE:STRING=RELEASE \
    -D CMAKE_VERBOSE_MAKEFILE:BOOL=OFF \
    -D Trilinos_ENABLE_TESTS:BOOL=OFF \
    -D Trilinos_ENABLE_Teuchos:BOOL=ON \
    -D BUILD_SHARED_LIBS:BOOL=ON \
    -D BLAS_LIBRARY_DIRS:PATH=/home/gidden/work/software/lapack-3.4.0 \
    -D BLAS_LIBRARY_NAMES:STRING="blas" \
    -D LAPACK_LIBRARY_DIRS:PATH=/home/gidden/work/software/lapack-3.4.0 \
    -D LAPACK_LIBRARY_NAMES:STRING="lapack" \
    -D CMAKE_INSTALL_PREFIX:PATH=$PWD \
    $EXTRA_ARGS \
    /home/gidden/work/software/trilinos-10.8.4-Source
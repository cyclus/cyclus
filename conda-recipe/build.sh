#!/bin/bash

mkdir build
cd build
export LD_LIBRARY_PATH=$PREFIX/lib/
export CMAKE_LIBRARY_PATH=$PREFIX/lib/
export PATH=$PREFIX/bin:$PATH

export MACOSX_DEPLOYMENT_TARGET=

# 
# Previous iterations have ahd trouble with hdf build vs. link
# versions. Something like the following has helped in the past.
#
#### hack fix for hdf5 issues
### if [[  `uname` == 'Linux' ]]; then
###   ln -s $PREFIX/lib/libhdf5.so.9 $PREFIX/lib/libhdf5.so.8
###   ln -s $PREFIX/lib/libhdf5_hl.so.9 $PREFIX/lib/libhdf5_hl.so.8
### else
###   ln -s $PREFIX/lib/libhdf5.9.dylib $PREFIX/lib/libhdf5.8.dylib
###   ln -s $PREFIX/lib/libhdf5_hl.9.dylib $PREFIX/lib/libhdf5_hl.8.dylib
### fi

if [[  `uname` == 'Linux' ]]; then
    cmake .. \
	-DCMAKE_INSTALL_PREFIX=$PREFIX \
	-DHDF5_ROOT=$PREFIX \
	-DBOOST_ROOT=$PREFIX \
	-DBOOST_LIBRARYDIR=$PREFIX/lib \
	-DBoost_NO_SYSTEM_PATHS=ON \
	-DCMAKE_BUILD_TYPE=Release \
	-DLAPACK_LIBRARIES=$PREFIX/lib/liblapack.so \
	-DBLAS_LIBRARIES=$PREFIX/lib/libblas.so
else
    echo $CFLAGS
    echo $LDFLAGS
    export MACOSX_DEPLOYMENT_TARGET=
    export DYLD_LIBRARY_PATH=$PREFIX/lib
    export LDFLAGS="-headerpad_max_install_names -headerpad"
    export CFLAGS="-headerpad_max_install_names -headerpad"
    export CXXFLAGS=
    cmake .. \
	-DCMAKE_INSTALL_PREFIX=$PREFIX \
	-DHDF5_ROOT=$PREFIX \
	-DCOIN_ROOT_DIR=$PREFIX \
	-DBOOST_ROOT=$PREFIX \
	-DCMAKE_BUILD_TYPE=Release \
	-DLAPACK_LIBRARIES=$PREFIX/lib/liblapack.dylib \
	-DBLAS_LIBRARIES=$PREFIX/lib/libblas.dylib
fi

make VERBOSE=1
make install

cd ..

#!/bin/bash

mkdir build
cd build
export LD_LIBRARY_PATH=$PREFIX/lib/
export CMAKE_LIBRARY_PATH=$PREFIX/lib/
export PATH=$PREFIX/bin:$PATH
export MACOSX_DEPLOYMENT_TARGET=

$PREFIX/bin/cyclus --version

if [[  `uname` == 'Linux' ]]; then
    cmake ..   \
	-DCMAKE_INSTALL_PREFIX=$PREFIX  \
	-DCYCLUS_ROOT_DIR=$PREFIX  \
	-DHDF5_ROOT=$PREFIX  \
	-DBOOST_ROOT=$PREFIX   \
	-DBOOST_LIBRARYDIR=$PREFIX/lib  \
	-DBoost_NO_SYSTEM_PATHS=ON  \
	-DCMAKE_BUILD_TYPE=Release  \
	-DLAPACK_LIBRARIES=$PREFIX/lib/liblapack.so  \
	-DBLAS_LIBRARIES=$PREFIX/lib/libblas.so
else
    export DYLD_FALLBACK_LIBRARY_PATH=$PREFIX/lib/cyclus:$PREFIX/lib
    cmake ..   \
	-DCMAKE_INSTALL_PREFIX=$PREFIX   \
	-DCYCLUS_ROOT_DIR=$PREFIX  \
	-DHDF5_ROOT=$PREFIX  \
	-DCOIN_ROOT_DIR=$PREFIX  \
	-DBOOST_ROOT=$PREFIX    \
	-DLAPACK_LIBRARIES=$PREFIX/lib/liblapack.dylib  \
	-DBLAS_LIBRARIES=$PREFIX/lib/libblas.dylib
fi

make 
make install

echo DONE

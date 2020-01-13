#!/bin/bash
export LD_LIBRARY_PATH=$PREFIX/lib
export CMAKE_LIBRARY_PATH=$PREFIX/lib
export PATH=$PREFIX/bin:$PATH
export DYLD_FALLBACK_LIBRARY_PATH=$PREFIX/lib/cyclus:$PREFIX/lib
UNAME=$(uname)

if [[ "${UNAME}" == 'Linux' ]]; then
  ln -s $PREFIX/lib/libhdf5.so.9 $PREFIX/lib/libhdf5.so.8
  ln -s $PREFIX/lib/libhdf5_hl.so.9 $PREFIX/lib/libhdf5_hl.so.8
else
  ln -s $PREFIX/lib/libhdf5.9.dylib $PREFIX/lib/libhdf5.8.dylib
  ln -s $PREFIX/lib/libhdf5_hl.9.dylib $PREFIX/lib/libhdf5_hl.8.dylib
fi

python setup.py install --prefix=$PREFIX --build-type=Release --hdf5=$PREFIX

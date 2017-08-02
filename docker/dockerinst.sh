#!/bin/bash
set -e
set -x

docker rm cyclus-local
docker run -w /cyclus --name cyclus-local -v $PWD:/cyclus cyclus/cyclus-deps \
  bash -c "rm -rf build-local && env LD_LIBRARY_PATH=/opt/conda/lib python install.py -j2 --build-type=Release --build_dir=build-local   -DBLAS_LIBRARIES=\"-L/opt/conda/lib -lopenblas\" -DLAPACK_LIBRARIES=\"-L/opt/conda/lib -llapack\""
docker commit cyclus-local cyclus/cyclus:local


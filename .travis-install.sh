#!/bin/bash

set -x
set -e

wget https://github.com/gidden/ciclus/archive/travis.zip -O ciclus.zip
unzip -j ciclus.zip "*/cyclus/*" -d conda-recipe

# build
conda build --no-test conda-recipe

# install
conda install --use-local cyclus=0.0

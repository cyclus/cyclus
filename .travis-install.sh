#!/bin/bash

set -x # print cmds
set -e # exit as soon as an error occurs

# log
msg=`git log --pretty=oneline -1`
echo "Building commit: $msg" 

# setup conda recipe
wget https://github.com/cyclus/ciclus/archive/master.zip -O ciclus.zip
unzip -j ciclus.zip "*/cyclus/*" -d conda-recipe

# build
conda build --no-test conda-recipe

# install
conda install --use-local cyclus=0.0

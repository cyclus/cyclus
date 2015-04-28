#!/bin/bash

set -x # print cmds
set -e # exit as soon as an error occurs

# log
msg=`git log --pretty=oneline -1`
echo "Building commit: $msg" 

# build
conda build --no-test conda

# install
conda install --use-local cyclus=0.0

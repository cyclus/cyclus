#!/bin/bash

docker rm cyclus-local
docker run -w /cyclus --name cyclus-local -v $PWD:/cyclus cyclus/cyclus-deps bash -c "mkdir -p Debug && cd Debug && cmake .. && make -j2 && make install"
docker commit cyclus-local cyclus/cyclus:local


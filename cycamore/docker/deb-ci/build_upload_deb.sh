#!/bin/bash

if [ $# -ne 1 ]; then 
  echo "illegal number of parameters"
  exit 1
fi

rm Dockerfile
sed -e s/CYCAMORE_UBUNTU_MAJOR_VERSION/${1}/g docker/deb-ci/Dockerfile_template > Dockerfile
sed -i s/CYCAMORE_REPO_HASH/`git rev-parse --short HEAD`/g Dockerfile

docker build -t deb/ubuntu-${1} . # build cyclus against ubuntu 14.04 in the docker container
docker create --name=deb-${1}.04 deb/ubuntu-${1} # build deb in docker
docker cp deb-${1}.04:/cycamore/deb ./ # extract deb
scp ./deb/cycamore_*_ubuntu.${1}.04.deb root@dory.fuelcycle.org:./cyclus-deb/ # push the deb on dory
rm -rf deb  # clean the deb folder

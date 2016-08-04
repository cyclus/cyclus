#!/bin/bash

if [ $# -ne 1 ]; then 
  echo "illegal number of parameters"
fi

rm Dockerfile
sed -e s/CYCLUS_UBUNTU_MAJOR_VERSION/${1}/g docker/release-ci/Dockerfile_template > Dockerfile__
sed -e s/CYCLUS_REPO_HASH/`git rev-parse --short HEAD`/g Dockerfile__ > Dockerfile
rm Dockerfile__

docker build -t deb/ubuntu-${1} . # build cyclus against ubuntu 14.04 in the docker container
docker create --name=deb-${1}.04 deb/ubuntu-${1} # build deb in docker
docker cp deb-${1}.04:/cyclus/deb ./ # extract deb
scp ./deb/cyclus_*_ubuntu.${1}.04.deb root@dory.fuelcycle.org:./cyclus-deb/ # push the deb on dory
rm -rf deb  # clean the deb folder

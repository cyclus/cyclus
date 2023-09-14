This Dockerfile supports two paths for building a docker image with Cyclus, one
that uses `conda` to install depenendencies and one that uses Ubuntu's `apt` to
install dependencies.

All of the docker images must be built from the top level directory of the
Cyclus code space.

To build the docker image in using `conda`:
`docker build --build-arg pkg_mgr=conda -f docker/Dockerfile .`

To build the docker image in using `apt`:
`docker build --build-arg pkg_mgr=apt -f docker/Dockerfile .`


Each subdirectory contains a Dockerfile that does something useful:

* ``develop-ci`` is the Dockerfile used for running Cycamore on a continuous
  integration service.  This Dockerfile assumes that the current working
  directory is a Cycamore repository - and that version of Cycamore is copied
  into the Docker container and used for the build.  The Dockerfile in the
  Cycamore repository root is a symbolic link to this Dockerfile.  This
  Dockerfile uses the base image ``cyclus/cyclus:latest`` from the Docker hub
  repository. This Docker container is uploaded as ``cyclus/Cycamore:latest`` on
  the Docker hub when merging a PR on the `develop` branch of the Cycamore Github
  repository.

* ``master-ci`` is the Dockerfile used for running last stable Cycamore version
  a continuous integration service. This Dockerfile works exactly as the
  develop-ci one, except that it builds Cycamore against the
  ``cyclus/cyclus:stable`` from the Docker hub repository. This Docker container
  is also uploaded on the Docker hub as the ``cyclus/Cycamore:stable`` when
  merging a PR on the `master` branch of Cycamore Github repository.

* ``deb-ci`` contains a script which allows to build the Dockerfile require
  to generate a Cycamore Debian package for the installation of Cycamore on
  ubuntu. The script take the major version number of ubuntu as a parameter. It
  builds Cycamore on the corresponding Ubuntu-LTS version, after installing all
  Cycamore dependencies -including Cyclus- and generates the Cycamore Debian
  installation package that need to be extracted from the Docker container
  (tested on Ubuntu 14.04 & 16.04)



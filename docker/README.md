
Each subdirectory contains a dockerfile that does something useful:

* ``cyclus-deps`` builds all cyclus dependencies.  This is used as the base
  image for other dockerfile's that build cyclus and should be updated only
  occasionally as needed and pushed up to the docker hub
  ``cyclus/cyclus-deps`` repository:

  ```
  cd cyclus-deps
  docker build -t cyclus/cyclus-deps:X.X .
  docker tag cyclus/cyclus-deps:X.X cyclus/cyclus-deps:latest
  docker push cyclus/cyclus-deps
  ```

* ``cyclus-ci`` is the dockerfile used for running cyclus on a continuous
  integration service.  This dockerfile assumes that the current working
  directory is a cyclus repository - and that version of cyclus is copied into
  the docker container and used for the build.  The dockerfile in the cyclus
  repository root is a symbolic link to this dockerfile.

* ``deb-ci`` is the dockerfile used to generate the Cyclus debian installation
  package. It contains 2 files, one Dockerfile_template and a script which:
    * replace the template variables to the appropriate values: the major ubuntu
      version require (provided as an parameter) and the commit hash tag in the
      Dockerfile_template (recover by the script),
    * runs the Dockerfile,
    * extract the debian package,
    * upload it on dory.fuelcycle.org.

The script ``dockercyclus.sh`` downloads (if not already downloaded before)
the cyclus/cycamore docker image and passes all given arguments to an cyclus
command run inside a docker container.  The current working directory is also
mounted inside the docker container so files in it (recursively) can be seen
by cyclus, and all output files end up in the host working directory.  This is
an example of an alternative distribution mechanism for cyclus.

The ``dockerbuild.sh`` script assumes the current working directory contains
the cyclus core repository and mounts it inside a docker container and builds
and installs the cyclus kernel.  The built docker image is saved as
cyclus/cyclus:local - which can be used to run tests, etc.  This could become
an easy way to onboard new kernel developers - they no longer have to set up a
fancy environment - all they have to do is clone cyclus and install docker.

The ``dockerinst.sh`` script is similar to ``dockerbuild.sh`` except that it
uses ``install.py`` to build and install cyclus.

Each subdirectory contains a dockerfile that does something useful:

* ``cyclus-deps`` builds all cyclus dependencies.  This is used as the base
  image for other dockerfile's that build cyclus and should be updated only
  occasionally as needed and pushed up to the docker hub
  ``cyclus/cyclus-deps`` repository:

  ```
  cd cyclus-deps
  docker build -t cyclus/cyclus-deps:X.X .
  docker tag cyclus/cyclus-deps:X.X cyclus/cyclus:latest
  docker push cyclus/cyclus-deps
  ```

* ``cyclus-ci`` is the dockerfile used for running cyclus on a continuous
  integration service.  This dockerfile assumes that the current working
  directory is a cyclus repository - and that version of cyclus is copied into
  the docker container and used for the build.  The dockerfile in the cyclus
  repository root is a symbolic link to this dockerfile.


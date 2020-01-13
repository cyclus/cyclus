Both fodler contain a script to generate the proper Dockerfile require to
generate the `stable` or the `latest` version. The `stable` version should be
generated when built from the last release tag and is build against
`cyclus/cycamore:stable` container. The `latest` verisonshould be build when
merging on `master` branch and is build against `cyclus/cycamore:latest`.



* ``cymetric-ci`` is the dockerfile used for running cymetric on a continuous
  integration service.  This dockerfile assumes that the current working
  directory is a cymetric repository - and that version of cymetric is copied
  into the docker container and used for the build.  The dockerfile in the
  cycamore repository root is a symbolic link to this dockerfile.  This
  dockerfile uses the base image ``cyclus/cycamore`` from the docker hub
  repository.

* ``cymetric-deps`` builds all cymetric dependencies. This should be done at
  each PR on each tag automatically.

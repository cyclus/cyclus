Cyclus/Cycamore Basic Release Procedure
=======================================

#. Update cyclus/src/version.h

#. `git tag -a <version>`

#. `git push upstream develop`

# `git push upstream <version>`

#. Update cycamore/src/version.h

#. `git tag -a <version>`

#. `git push upstream develop`

# `git push upstream <version>`

#. Follow instructions in cycamore/tests/README.rst

#. `git push upstream develop`

#. `git tag -fa <version>`

#. `git push -f upstream <version>`

#. Log onto `cyclusci@submit-1.batlab.org`

#. `cd ciclus`

#. update the version number and git_tag in `cyclus/meta.yaml` and `cycamore/meta.yaml`

#. ./submit.sh cycamore.run-spec

#. wait for the job to finish

#. ./upload_conda.sh <nmi_directory> <version>
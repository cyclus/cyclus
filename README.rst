Cyclus
------
.. image:: https://coveralls.io/repos/github/cyclus/cyclus/badge.svg
  :target: https://coveralls.io/github/cyclus/cyclus


The Cyclus fuel cycle simulator is an agent-based and extensible framework for
modeling the flow of material through future nuclear fuel cycles.  For more
information on the entire "ecosystem" please refer to the `Cyclus website
<http://fuelcycle.org>`_.


Cyclus Projects Status
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+--------------------+--------------------+--------------------+--------------------+
| Branch             | Cyclus             | Cycamore           | Cymetric           |
+====================+====================+====================+====================+
| ``main``           | |cyclus_main|      | |cycamore_main|    | |cymetric_main|    |
+--------------------+--------------------+--------------------+--------------------+

.. |cyclus_main| image:: https://github.com/cyclus/cyclus/actions/workflows/publish_latest.yml/badge.svg
.. |cycamore_main| image:: https://github.com/cyclus/cycamore/actions/workflows/publish_latest.yml/badge.svg
.. |cymetric_main| image:: https://github.com/cyclus/cymetric/actions/workflows/publish_latest.yml/badge.svg



###########
Cyclus Core
###########

The core of the Cyclus nuclear fuel cycle simulator from the
University of Wisconsin-Madison is intended to be a simulation
framework for the development of innovative fuel cycle simulations.

This README is intended primarily for those who intend to contribute to the
development of the Cyclus Core.  If you are interested Cyclus as a user or in
developing Cyclus archetypes, you may want to consult `Getting Started with Cyclus <http://fuelcycle.org/user/install.html>`_.

This README provides basic information about:
 - the dependencies required by Cyclus
 - installation of Cyclus from the command line
 - building Cyclus from source for development
 - how to run Cyclus and the Cyclus unit tests
 - setting up an environment for developing Cyclus archetypes

The Cyclus Core contains all the fundamental pieces of the Cyclus framework
required to interface with the input file, write the output file, and manage
material flow during the simulation via the Dynamic Resource Exchange.  It
does not contain any fuel cycle facility models. A set of nuclear facilities can be
obtained by installing Cycamore, the Cyclus Additional Module.  Cycamore is
supported by the Cyclus Developer Team.

Third party modules can also be installed (or developed) with additional
facilities.  Please visit the Cyclus website for a `list of contributed modules <http://fuelcycle.org/user/index.html#archetypes>`_.

- **For general information about Cyclus, visit the**  `Cyclus Homepage`_,

- **For detailed installation instructions, visit the**
  `INSTALLATION Guide <INSTALL.rst>`_,

- **To see user and developer documentation for this code, please visit
  the** `Users Guide <http://fuelcycle.org/user/index.html>`_,

- **If you would like to contribute to Cyclus, please check our**
  `Contribution Guidelines <CONTRIBUTING.rst>`_.


.. contents:: Table of Contents


************
Dependencies
************

In order to facilitate future compatibility with multiple platforms,
Cyclus is built using ``CMake``. For detailed instructions on Cyclus dependencies (including minimum version requirements), see `Installing Dependencies <DEPENDENCIES.rst>`_.

When building from source with `Pixi <https://pixi.prefix.dev/>`_ (see
`Building Cyclus from Source`_) all of the dependencies are installed for you
from conda-forge, and the only thing you need to install yourself is Pixi.

*************************
Quick Cyclus Installation
*************************

The quickest way to install Cyclus is as a conda package from `conda-forge
<https://github.com/conda-forge/cyclus-feedstock>`_, where it is available for
Linux (x86-64 and ARM64) and macOS (Apple Silicon and x86-64) platforms.

With `Pixi <https://pixi.prefix.dev/>`_ installed, create a workspace for your
simulations and add Cyclus to it:

.. code-block:: console

  pixi init my-simulations
  cd my-simulations
  pixi add cyclus

Run commands in the environment of the workspace with ``pixi run`` (e.g.
``pixi run cyclus input.xml``), or use ``pixi shell`` to get a shell with the
environment activated.

Alternatively, with `conda <https://docs.conda.io/projects/conda/>`_, create a
globally activated environment for Cyclus and then activate it:

.. code-block:: console

  conda create -n cyclus -c conda-forge cyclus
  conda activate cyclus

For more detailed installation procedure, and/or custom installation please
refer to the `INSTALLATION guide <INSTALL.rst>`_.

***************************
Building Cyclus from Source
***************************

The repository is a `Pixi <https://pixi.prefix.dev/>`_ workspace driven by a
Pixi manifest (``pixi.toml``) that provides all of the dependencies of Cyclus
from conda-forge, with the exact versions recorded in ``pixi.lock``.
This is the recommended way to build Cyclus for development.
With Pixi installed, build Cyclus and install it into the local ``install``
directory with:

.. code-block:: console

  git clone https://github.com/cyclus/cyclus.git
  cd cyclus
  pixi run build

and run the tests with:

.. code-block:: console

  pixi run test

Use ``pixi shell`` to get a shell with the environment activated, which also
puts the installed ``cyclus`` on ``PATH``.
Run ``pixi task list`` to see all of the predefined tasks of the workspace.
To build Cyclus with the dependencies of your system instead, see the
`INSTALLATION guide <INSTALL.rst>`_.

*************
Running Tests
*************

Installing Cyclus will also install a test driver (i.e., an executable of all of
our tests).
You can run the tests yourself in an environment with Cyclus installed via:

.. code-block:: console

    cyclus_unit_tests

When building from source with Pixi, ``pixi run test`` runs both these tests
and the Python tests.

*******************
Installing Cycamore
*******************

As noted previously, the Cyclus Core will not allow you to run fuel cycle
simulations as it does not include nuclear facilities.
To run fuel cycle simulations you also need Cycamore.
For how to install it, see the `GitHub Cycamore Repository
<https://github.com/cyclus/cycamore>`_.

*********************
Developing Archetypes
*********************

New archetypes are developed in their own project, which is built against an
installed Cyclus.
The ``cycstub`` command creates a skeleton project to start from.

Developing Against a Nightly Build of Cyclus
============================================

The ``main`` branch of Cyclus is built as a conda package and published to the
``https://prefix.dev/cyclus/nightly`` channel, for Linux (x86-64 and ARM64) and
macOS (Apple Silicon), as described in the `nightly-builds-conda repository
<https://github.com/cyclus/nightly-builds-conda>`_.
With `Pixi <https://pixi.prefix.dev/>`_ installed, create a workspace for the
archetype project, add the nightly channel with channel priority, and add
Cyclus and the build tools to it:

.. code-block:: console

  pixi init my-archetype
  cd my-archetype
  pixi workspace channel add --prepend https://prefix.dev/cyclus/nightly
  pixi add cyclus cxx-compiler cmake make

To let Cyclus find the archetypes that are installed into the ``install``
directory of the workspace, add it to the ``CYCLUS_PATH`` environment variable
in the ``pixi.toml`` Pixi manifest:

.. code-block:: toml

  [activation.env]
  CYCLUS_PATH = "$PIXI_PROJECT_ROOT/install/lib/cyclus"

Then create the project, build and install it, and run its example input file:

.. code-block:: console

  pixi run cycstub --type facility my_archetype:my_archetype:MyFacility
  pixi run python install.py --prefix install
  pixi run cyclus example_my_facility.xml

Developing Against Cyclus Built from Source
===========================================

Alternatively, to develop an archetype against Cyclus built from this
repository, build Cyclus and register its workspace with Pixi under the name
``cyclus-archetype-dev``, so that its environment can be used from any
directory:

.. code-block:: console

  pixi run build
  pixi workspace register --name cyclus-archetype-dev

If the name ``cyclus-archetype-dev`` is already registered, e.g. for another
checkout of Cyclus, choose another name and use it in place of
``cyclus-archetype-dev`` with ``-w`` below.
Then create the project in a directory outside of the repository, and build
and install it with the environment of the ``cyclus-archetype-dev``
workspace:

.. code-block:: console

  mkdir ../my-archetype
  cd ../my-archetype
  pixi run -w cyclus-archetype-dev cycstub --type facility my_archetype:my_archetype:MyFacility
  pixi run -w cyclus-archetype-dev python install.py --prefix install

Add the directory ``install/lib/cyclus`` of the project to the ``CYCLUS_PATH``
environment variable, so that Cyclus can find the archetype, and run the
example input file:

.. code-block:: console

  export CYCLUS_PATH="$PWD/install/lib/cyclus"
  pixi run -w cyclus-archetype-dev cyclus example_my_facility.xml

For more on writing archetypes, see the `Archetype Developer Guide
<http://fuelcycle.org/arche/index.html>`_.


**************
Running Cyclus
**************

You can find instructions for writing an input file for cyclus from `Cyclus User
Guide`_ or use sample input files from `Cycamore Repo`_. Assuming you have some
file ``input.xml``, you can run Cyclus via:

.. code-block:: console

    cyclus path/to/input.xml

For a more detailed explanation, check out the `Cyclus User Guide`_.

************
Contributing
************

We happily welcome new developers into the Cyclus Developer Team. If you are willing
to contribute into Cyclus, please follow this procedure:

#. Fork Cyclus repository,

#. Create a working branch on your fork from the ``main`` branch,

#. Implement your modification of the Cyclus source code,

#. Submit a Pull request into ``Cyclus/main`` branch,

#. Wait for reviews/merge (the proposer of a pull request cannot be the Merger).

You may also want to read our `Contribution Guidelines <CONTRIBUTING.rst>`_.

.. _`Cyclus Homepage`: http://fuelcycle.org/
.. _`Cyclus User Guide`: http://fuelcycle.org/user/index.html
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cycamore Repo`: https://github.com/cyclus/cycamore
.. _`INSTALL`: INSTALL.rst
.. _`CONTRIBUTING`: CONTRIBUTING.rst

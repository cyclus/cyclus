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
 - how to run Cyclus and the Cyclus unit tests

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

*************************
Quick Cyclus Installation
*************************
The quickest way to install Cyclus and its dependencies relies on using the `conda-forge` channel and the `conda` package manager within the Anaconda python environment.  The following instructions guide you through that approach.
To install Cyclus and its dependencies onto a clean Ubuntu machine (tested on 18.04 LTS):

- Download the latest Anaconda installer for Linux at
  ``https://www.anaconda.com/distribution/#download-section``

- Move the ``.sh`` to your Home directory

- In Terminal, execute the following commands:

- ``bash Anaconda3-2019.03-Linux-x86_64.sh``

- ``echo 'export PATH="~/anaconda/bin:$PATH"' >> ~/.bashrc``

- ``source .bashrc``

- ``conda config --add channels conda-forge``

- ``conda create -n cyclus python=3.11``

- ``conda activate cyclus``

- ``conda install -y openssh gxx_linux-64 gcc_linux-64 cmake make docker-pycreds git xo python-json-logger glib glibmm zlib libxml2 libxmlpp libxmlpp-4.0 libblas libcblas liblapack pkg-config coincbc boost-cpp hdf5 sqlite pcre gettext bzip2 xz setuptools pytest pytables pandas jinja2 "cython<3" websockets pprintpp``

- ``conda install -y --force-reinstall libsqlite``

- Use ``sudo apt install`` to install and configure git

- Clone the Cyclus repository by running ``git clone https://github.com/cyclus/cyclus.git``

- Navigate to the folder containing Cyclus

- Run the command ``python install.py``

For more detailed installation procedure, and/or custom installation please
refer to the `INSTALLATION guide <INSTALL.rst>`_.

*************
Running Tests
*************

Installing Cyclus will also install a test driver (i.e., an executable of all of
our tests). You can run the tests yourself via:

.. code-block:: bash

    $ cyclus_unit_tests


*******************
Installing Cycamore
*******************

As noted previously, the Cyclus Core will not allow you to run fuel cycle
simulations as it does not include nuclear facilities. To run fuel cycle
simulations, first download Cycamore
(`GitHub Cyamore Repository <https://github.com/cyclus/cycamore>`_):

- Clone the Cycamore Repo: ``git clone
  https://github.com/cyclus/cycamore.git``,

- Install in the same location you installed Cyclus (eg. in ``~/.local/``):
  ``python install.py`` from the ``cycamore`` folder.


**************
Running Cyclus
**************

You can find instructions for writing an input file for cyclus from `Cyclus User
Guide`_ or use sample input files from `Cycamore Repo`_. Assuming you have some
file ``input.xml``, you can run Cyclus via:

.. code-block:: bash

    $ cyclus path/to/input.xml

For a more detailed explanation, check out the `Cyclus User Guide`_.

************
Contributing
************

We happily welcome new developers into the Cyclus Developer Team. If you are willing
to contribute into Cyclus, please follow this procedure:

#. Fork Cyclus repository,

#. Create a working branch on your fork from the ``develop`` branch,

#. Implement your modification of the Cyclus source code,

#. Submit a Pull request into ``Cyclus/develop`` branch,

#. Wait for reviews/merge (the proposer of a pull request cannot be the Merger).

You may also want to read our `Contribution Guidelines <CONTRIBUTING.rst>`_.

.. _`Cyclus Homepage`: http://fuelcycle.org/
.. _`Cyclus User Guide`: http://fuelcycle.org/user/index.html
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cycamore Repo`: https://github.com/cyclus/cycamore
.. _`INSTALL`: INSTALL.rst
.. _`CONTRIBUTING`: CONTRIBUTING.rst

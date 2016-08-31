###########
Cyclus Core
###########

The core of the Cyclus nuclear fuel cycle simulator from the
University of Wisconsin-Madison is intended to be a simulation
framework for the development of innovative fuel cycle simulations.

This readme provides basic information about:
 - the dependencies required by Cyclus 
 - the Cyclus installation command line
 - how to run Cyclus and the Cyclus unit tests

The Cyclus Core contains all the fundamental pieces of the Cyclus framework
required to interface with the input file, write the output file, and manage
material flow during the simulation via the Dynamic Resource Exchange... but it
does not contain any fuel cycle facilities. A set of nuclear facilities can be
obtained by installing Cycamore, the Cyclus Additionnal Module.  Cycamore is
supported by the Cyclus Developer Team.

Third party modules can also be installed (or developed) with additional
facilities.  Please visit the Cyclus website for a list of contributed modules.

- **For general information about Cyclus, visit the**  `Cyclus Homepage`_

- **For detailed installation instructions, visit the**
  `INSTALLATION Guide <INSTALL.rst>`_.

- **To see user and developer documentation for this code, please visit
  the** `Users Guide <http://fuelcycle.org/user/index.html>`_ 

- **If you would like to contribute to Cyclus, please check our** 
  `Contribution Guidelines <CONTRIBUTING.rst>`_.


.. contents:: Table of Contents


******************************
Dependencies
******************************

In order to facilitate future compatibility with multiple platforms,
Cyclus is built using `CMake`_. A full list of the Cyclus package
dependencies is shown below:

====================   ==================
Package                Minimum Version
====================   ==================
`CMake`                2.8
`boost`                1.46.1
`libxml2`              2
`libxml++`             2.36
`python`               2.7 or 3.3+
`sqlite3`              3.7.10
`HDF5`                 1.8.4
`Coin-Cbc`             2.5
====================   ==================

For detailed instructions on installing dependencies, see `Installing Dependencies <DEPENDENCIES.rst>`_

******************************
Quick Cyclus Installation
******************************

Assuming you have the dependencies installed correctly, installing Cyclus using
github is fairly straightforward:

- Clone the Cyclus Repo: ``git clone https://github.com/Baaaaam/cyclus.git``

- to install Cyclus locally (in ``~/.local/``) just run: ``python install.py``
  from cyclus folder

- finally, add the following Cyclus installation path (``~/.local/cyclus``) to
  the **bottom** on your ``$PATH``.

For more detailed installation procedure, and/or custom installation please
refer to the `INSTALLATION guide <INSTALL.rst>`_


******************************
Running Tests
******************************

Installing Cyclus will also install a test driver (i.e., an executable of all of
our tests). You can run the tests yourself via:

.. code-block:: bash

    $ cyclus_unit_tests


******************************
Installing Cycamore
******************************

As noted previously, the Cyclus Core will not allow you to run fuel cycle
simulations as it does not include nuclear facilities. To run fuel cycle
simulations, first download Cycamore
(`GitHub Cyamore Repository <https://github.com/cyclus/cycamore>`_ )

- Clone the Cycamore Repo: ``git clone https://github.com/Baaaaam/cycamore.git``

- Install in the same location you installed Cyclus (eg. in ``~/.local/``):
  ``python install.py`` from the cycamore folder


******************************
Running Cyclus
******************************

You can find instructions for writing an input file for cyclus from `Cyclus User
Guide`_ or use sample input files from `Cycamore Repo`_. Assuming you have some
file `input.xml`, you can run Cyclus via:

.. code-block:: bash

    $ cyclus path/to/input.xml

For a more detailed explanation, check out the `Cyclus User Guide`_.

.. _`CMake`: https://cmake.org
.. _`Cyclus Homepage`: http://fuelcycle.org/
.. _`Cyclus User Guide`: http://fuelcycle.org/user/index.html
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cycamore Repo`: https://github.com/cyclus/cycamore
.. _`INSTALL`: INSTALL.rst
.. _`CONTRIBUTING`: CONTRIBUTING.rst


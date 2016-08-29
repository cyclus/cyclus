###########
Cyclus Core
###########

The core of the Cyclus nuclear fuel cycle simulator from the
University of Wisconsin - Madison is intended to be a simulation
framework upon which to develop innovative fuel cycle simulations.

- This readme provides basics informations about:
 - the dependencies required by Cyclus, 
 - the Cyclus installation command line,
 - how to run Cyclus and the cyclus unit test.

- **For detailed installation instruction, visit the** `Cyclus Website
  <http://fuelcycle.org/user/index.html>`_ **or** 
  `INSTALLATION guide <INSTALL.rst>`_.

- **To see user and developer documentation for this code, please visit
  the** `Cyclus Homepage`_.

- **If you would like to contribute to Cyclus, please check our** 
  `contributionguidelines <CONTRIBUTING.rst>`_.


The Cyclus Core contain all the fundamental pieces of the Cyclus framework
allowing interfacing the input file, the writting of the output file, the
management of the material flux, the Dynamical Ressource Exchange... but it did
not contain any physical facilities. A set of facilities  can be obtain
installing Cycamore, the Cyclus Additionnal Modules. The Cycamore module is
supported by the Cyclus Developpers Team.

Other modules can be installed (or developped) to use other facilities
(with or without the Cycamore facilities).  Please visit the Cyclus website to
have a more exhaustiv list of the available modules.



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

If you need some help to install those dependencies please check our
dependencies notices available `here <DEPENDENCIES.rst>`_ or `Cyclus Website <http://fuelcycle.org/user/index.html>`_.


******************************
Quick Cyclus Installation
******************************

Assuming you have the dependencies installed correctly, installing Cyclus is
fairly straightforward:

- Clone the Cyclus Repo: `git clone https://github.com/Baaaaam/cyclus.git`, 

- to install Cyclus locally (in ``~/.local/``) just run: ``python install.py``
from cyclus folder,

- finally, add the following Cyclus installation path (``~/.local/cyclus``) to the
**bottom** on your ``$PATH``.

For more detailed installation procedure, and/or custom installation please
refer to the `INSTALLATION guide <INSTALL.rst>`_ (also available on `Cyclus
Website <http://fuelcycle.org/user/index.html>`_)

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

As explain previously, the Cyclus Core will not allow you to run simulation as
ti does not include facilities. The Cyclus developper Team support the
development of facility modules, Cycamore (Cyclus Additionnal Modules
Repositiory), can be downloaded from 
`GitHub <https://github.com/Baaaaam/cycamore>`. 


******************************
Running Cyclus
******************************

You can find instructions for writing an input file for cyclus from `Cyclus User
Guide`_ or use sample input files from `Cycamore Repo`_. Assuming you have some
file `input.xml`, you can run Cyclus via:

.. code-block:: bash

    $ cyclus path/to/input.xml

For a more detailed explanation, checkout the `Cyclus User Guide`_.

.. _`CMake`: https://cmake.org
.. _`Cyclus Homepage`: http://fuelcycle.org/
.. _`Cyclus User Guide`: http://fuelcycle.org/user/index.html
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cycamore Repo`: https://github.com/cyclus/cycamore
.. _`INSTALL`: INSTALL.rst
.. _`CONTRIBUTING`: CONTRIBUTING.rst


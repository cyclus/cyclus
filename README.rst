###########
Cyclus Core
###########

The core of the Cyclus nuclear fuel cycle simulator from the
University of Wisconsin - Madison is intended to be a simulation
framework upon which to develop innovative fuel cycle simulations.

**To see user and developer documentation for this code, please visit
the** `Cyclus Homepage`_.

**If you would like to contribute to Cyclus, please check our** `contribution
guidelines <CONTRIBUTING.rst>`_.

**For detailed installation instruction, visit the** `Cyclus Website
<http://fuelcycle.org/user/index.html>`_ **or** `INSTALLATION guide
<INSTALL.rst>`_.


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

Other installation method and detailed installation procedure
can be found in `INSTALL`_ or on the `Cyclus website`_.


Quick Cyclus Installation
=============================================

Assuming you have the dependencies installed correctly, installing Cyclus is
fairly straightforward.

To install Cyclus locally (in ``~/.local/``) just run: ``python install.py``

Finally, add the following Cyclus installation path (``~/.local/cyclus``) to the
**bottom** on your ``$PATH``.

For more detailed installation procedure, and/or custom installation please
refer to the `INSTALLATION guide <INSTALL.rst>`_ (also available on `Cyclus
Website <http://fuelcycle.org/user/index.html>`_)

Running Tests
=============

Installing Cyclus will also install a test driver (i.e., an executable of all of
our tests). You can run the tests yourself via:

.. code-block:: bash

    $ cyclus_unit_tests

Running Cyclus
==============

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
.. _`Cyclus website`: http://fuelcycle.org/user/install.html
.. _`INSTALL`: INSTALL.rst
.. _`CONTRIBUTING`: CONTRIBUTING.rst


###########
Cyclus Installation Guide
###########

Cyclus uses the CMake cross platformbuild-generator as a build system. TO
simplify the usage of cmake to build Cyclus from source, a python script is
provided with the source.

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

You can find `here <DEPENDENCIES.rst>`_  help to install those dependencies for
the major supported system.

******************************
Installation
******************************

Before going further on the installation proccedure be sure you have installied
all the required dependencies listed above. You can also find `there
<DEPENDENCIES.rst>`_ instruction to install those dependencies depending of the
system you are working on.

Installing Cyclus from source:
=============================================
 
Once you have installed all the dependency you can follow those step the
complete the default installation of Cyclus:

#. Create a folder to host the Cyclus environment: 
.. code-block:: bash
   mkdir cyclusENV && cd cyclusENV
#. Download the source from there
   or from the Git-repository: 
.. code-block:: bash
   git clone https://github.com/cyclus/cyclus . && git fetch && git checkout master
#. Move into the new Cyclus directory:``cd cyclus``
#. Run the install script:
.. code-block:: bash
   python install.py


If you successfully followed the instruction above cyclus binanry have been
generated and be placed in the  ``.local/`` in your home directory. 
You need to had ``~/.local/bin`` to the bottom of your ``$PATH``:
.. code-block:: bash
  $> echo 'export PATH="$HOME/.local/bin:$PATH' >> .bashrc

Custom Cyclus installation
============================================

The installation using the install script can be customized using the following
flag:

#.  ``-h, --help``                         show the help message and exit
#.  ``--build_dir BUILD_DIR``              where to place the build directory
#.  ``--uninstall``                        uninstall
#.  ``--no-update``                        do not update the hash in version.cc
#.  ``--clean-build``                      attempt to remove the build directory before building
#.  ``-j THREADS, --threads``              THREADS the number of threads to use in the make step
#.  ``--prefix PREFIX``                    the relative path to the installation directory
#.  ``--config-only``                      only configure the package, do not build or install
#.  ``--build-only``                       only build the package, do not install
#.  ``--test``                             run tests after building
#.  ``--coin_root COIN_ROOT``              the relative path to the Coin-OR libraries directory
#.  ``--boost_root BOOST_ROOT``            the relative path to the Boost libraries directory
#.  ``--hdf5_root HDF5_ROOT``              the path to the HDF5 libraries directory
#.  ``--cmake_prefix_path CMAKE_PREFIX_PATH`` the cmake prefix path for use with FIND_PACKAGE, FIND_PATH, FIND_PROGRAM, or FIND_LIBRARY macros
#.  ``--build_type BUILD_TYPE`` change  the CMAKE_BUILD_TYPE
#.  ``-D VAR``                Set enviornment variable(s).

Assuming you have the dependencies installed correctly, installing Cyclus is
fairly straightforward.

We make the following assumptions in this guide:

#. there is some master directory in which you're placing all
   Cyclus-related files called .../cyclus
#. you want to install cyclus **locally** (in ``~/.local``)
#. you have acquired the Cyclus source code from the `Cyclus repo`_
#. you have placed the Cyclus repository in .../cyclus/cyclus

Under these assumptions **and** if you used a package manager to
install coin-Cbc (i.e. it's installed in a standard location), the
Cyclus building and installation process will look like:

.. code-block:: bash

    .../cyclus/cyclus$ python install.py

If you have installed coin-Cbc from source or otherwise have it
installed in a non-standard location, you should make use of the
coinRoot installation flag. The otherwise identical process would look
like:

.. code-block:: bash

    .../cyclus/cyclus$  python install.py --coin_root=path/to/coin

Additionally, if you have installed Boost in a non-standard location
you should make use of the boostRoot installation flag.

.. code-block:: bash

    .../cyclus/cyclus$ python install.py --coin_root=/path/to/coin --boost_root=/path/to/boost

There are additional options which can be inspected via `install.py`'s help:

.. code-block:: bash

    .../cyclus/cyclus$ python install.py -h

Finally, add the following line to the **bottom** your ``~/.bashrc`` file
(``~/.bash_profile`` on Macs):

.. code-block:: bash

    export PATH="$HOME/.local/bin:$PATH"

Then update your environment

.. code-block:: bash

    $ source ~/.bashrc

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

For a more detailed explanation, checkout the user guide.

.. _`Cyclus Homepage`: http://fuelcycle.org/
.. _`Cyclus User Guide`: http://fuelcycle.org/user/index.html
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cycamore Repo`: https://github.com/cyclus/cycamore


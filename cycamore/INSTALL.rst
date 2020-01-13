###########################
Cycamore Installation Guide
###########################

Cycamore uses the CMake cross-platform build-generator. To
simplify the usage of cmake to build Cycamore from source, a python script is
provided with the source.

.. contents:: Table of Contents

************
Dependencies
************

Cycamore's only dependency is the Cyclus Core.

====================   ==================
Package                Minimum Version
====================   ==================
`Cyclus`               1.4
====================   ==================

There are a number of ways to install the Cyclus core:

- To install from source code, see the `Cyclus Core repository
  <http://github.com/cyclus/cyclus>`_

- To install from a binary distribution, see the instructions for
  `Installing Cyclus from Binaries <DEPENDENCIES.rst>`_


******************************
Installation
******************************
.. website_include_start

Before going further with the installation procedure, be sure you have installed
`Cyclus <http://github.com/cyclus/cyclus>`_.


Default Installation
------------------------

Run the install script:

.. code-block:: bash
  
  python install.py


If you successfully followed the instructions above, then the Cycamore library
has been generated and placed in ``.local/`` in your home directory. 

.. website_include_end

.. website_custom_start

Custom Cycamore Installation
----------------------------

The installation using the install script can be customized using the following
flag:

.. list-table::

  * - `-h`, `--help`
    - show this help message and exit

  * - `--build_dir BUILD_DIR`
    - where to place the build directory

  * - `--uninstall`
    - uninstall

  * - `--no-update`
    - do not update the hash in version.cc

  * - `--clean-build`
    - attempt to remove the build directory before building

  * - `-j THREADS`, `--threads THREADS`
    - the number of threads to use in the make step

  * - `--prefix PREFIX`
    - the relative path to the installation directory

  * - `--config-only`
    - only configure the package, do not build or install

  * - `--build-only`
    - only build the package, do not install

  * - `--test`
    - run tests after building

  * - `--allow-milps`
    - Allows mixed integer linear programs by default

  * - `--dont-allow-milps`
    - Don't Allows mixed integer linear programs by default

  * - `--deps-root DEPS_ROOT`, `--deps_root DEPS_ROOT`
    - the path to the directory containing all dependencies

  * - `--coin-root COIN_ROOT`, `--coin_root COIN_ROOT`
    - the relative path to the Coin-OR libraries directory

  * - `--boost_root BOOST_ROOT`
    - the relative path to the Boost libraries directory

  * - `--hdf5_root HDF5_ROOT`
    - the path to the HDF5 libraries directory

  * - `--cyclus-root CYCLUS_ROOT`, `--cyclus_root CYCLUS_ROOT`
    - the relative path to Cyclus installation directory

  * - `--cmake_prefix_path CMAKE_PREFIX_PATH`
    - the cmake prefix path for use with FIND_PACKAGE, FIND_PATH, FIND_PROGRAM, or FIND_LIBRARY macros

  * - `--build-type BUILD_TYPE`, `--build_type BUILD_TYPE`
    - the CMAKE_BUILD_TYPE

  * - `-D VAR`
    - Set enviornment variable(s).


For example, if you have installed coin-Cbc from source or otherwise have it
installed in a non-standard location, you should make use of the coinRoot
installation flag. The otherwise identical process would look like:

.. code-block:: bash

    .../cycamore$  python install.py --coin_root=path/to/coin


CMake Cycamore Installation
---------------------------

If you are ``CMake`` aficionado you can also install Cycamore without using the
install.py python script and use ``cmake`` directly, which should look like:


.. code-block:: bash

  mkdir bld
  cd build
  cmake .. -DCMAKE_INSTALL_PREFIX=~/.local/
  make
  make install

You can customize your cmake installation using the proper cmake flag.  All
cmake variables can be listed using: ``cmake -LAH``.  The main variables used are:

.. list-table::

  * - ``COIN_ROOT_DIR`` 
    - set Coin-OT library directory 

  * - ``BOOST_ROOT``    
    - set Boost liraries directory

  * - ``HDF5_ROOT``     
    - set HDF5 root directory

  * - ``HDF5_LIBRARIES`` 
    - set HDF5 libraries path

  * - ``HDF5_LIBRARY_DIRS`` 
    - set HDF5 library directory

  * - ``HDF5_INCLUDE_DIRS`` 
    - set HDF5 include directory

All variables can be set using ``-DMY_VARIABLE=MY_VARIABLES_VALUE``.

.. website_custom_end

******************************
Running Tests
******************************

Installing Cycamore will also install a test driver (i.e., an executable of all of
our tests). We strongly recommend after a fresh install of Cycamore, or after
any modification on the source code, to rerun those tests in order to insure the
proper functioning of Cycamore. You can run the tests yourself via:

.. code-block:: bash

    $ cycamore_unit_tests


.. _`Cyclus Homepage`: http://fuelcycle.org/
.. _`Cyclus User Guide`: http://fuelcycle.org/user/index.html
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cycamore Repo`: https://github.com/cyclus/cycamore


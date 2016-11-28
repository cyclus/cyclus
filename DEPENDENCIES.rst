###########
Cyclus Core
###########

The core of the Cyclus nuclear fuel cycle simulator from the
University of Wisconsin - Madison is intended to be a simulation
framework upon which to develop innovative fuel cycle simulations.

To see user and developer documentation for this code, please visit
the `Cyclus Homepage`_.

.. contents:: Table of Contents
   :depth: 2

************
Dependencies
************

.. website_include_start

In order to facilitate future compatibility with multiple platforms,
Cyclus is built using ``CMake``. A full list of the Cyclus package
dependencies is shown below:

====================   ==================
Package                Minimum Version
====================   ==================
``CMake``                2.8
``boost``                1.46.1
``libxml2``              2
``libxml++``             2.36
``python``               2.7 or 3.3+
``sqlite3``              3.7.10
``HDF5``                 1.8.4
``Coin-Cbc``             2.5
====================   ==================

.. website_include_end

On some platforms, such as Ubuntu 16.04, the following are also necessary:

====================   ==================
Package                Minimum Version
====================   ==================
``g++``                  4.8.2
``libblas-dev``          1.2
``liblapack-dev``        3.5.0
====================   ==================


And a few optional dependencies:

====================   ==================
Package                Minimum Version
====================   ==================
doxygen (for docs)     1.7.6.1
tcmalloc (for speed)   any
Cython                 0.13.1
Python (dev version)   2.7 or 3.3+
Jinja2                 any
NumPy                  1.9+
Pandas                 any
====================   ==================

*Note that the Debian/Ubuntu package ``libtcmalloc`` is NOT discovered correctly
by our build system.  Instead use ``libgoogle-perftools-dev``.*

*Also note that the development version of Python, Jinja2, NumPy, and Pandas are
only needed if Cython is installed.*

***********************
Installing Dependencies
***********************

Cyclus dependencies can either be installed mannualy or using an operating system's package
manager.

Installing Dependencies (Linux and Unix)
----------------------------------------

This guide assumes that the user has root access (to issue ``sudo`` commands) and
access to a package manager or has some other suitable method of automatically
installing established libraries. This process was tested using a fresh install
of Ubuntu versions 16.04, using ``apt-get`` as the package
manager (scroll down further for Mac OSX instructions).

The command to install a dependency takes the form of:

.. code-block:: bash

  sudo apt-get install package

where "package" is replaced by the correct package name. The minimal list of
required library package names is:

#. make
#. cmake
#. libboost-all-dev (see note below)
#. libxml2-dev
#. libxml++2.6-dev
#. libsqlite3-dev
#. libhdf5-serial-dev
#. libbz2-dev
#. coinor-libcbc-dev
#. coinor-libcoinutils-dev
#. coinor-libosi-dev
#. coinor-libclp-dev
#. coinor-libcgl-dev

and (optionally):

#. doxygen
#. g++
#. libblas-dev
#. liblapack-dev
#. libgoogle-perftools-dev
#. python3
#. python3-dev
#. python3-tables
#. python3-pandas
#. python3-numpy
#. python3-nose
#. python3-jinja2
#. cython3

For example, in order to install libxml++ (and libxml2) on your system, type:

.. code-block:: bash

  sudo apt-get install libxml++2.6-dev

If you'd prefer to copy/paste, the following line will install all *Cyclus*
dependencies:

.. code-block:: bash

   sudo apt-get install -y cmake make libboost-all-dev libxml2-dev libxml++2.6-dev libsqlite3-dev libhdf5-serial-dev libbz2-dev coinor-libcbc-dev coinor-libcoinutils-dev coinor-libosi-dev coinor-libclp-dev coinor-libcgl-dev libblas-dev liblapack-dev g++ libgoogle-perftools-dev

Boost Note
^^^^^^^^^^

The ``libboost-all-dev`` used above will install the entire Boost library, which
is not strictly needed. We currently depend on a small subset of the Boost
libraries:

#. libboost-program-options-dev
#. libboost-system-dev
#. libboost-filesystem-dev

However, it is possible (likely) that additional Boost libraries will be used
because they are an industry standard. Accordingly, we suggest simply installing
``libboost-all-dev`` to limit any headaches due to possible dependency additions
in the future.

Installing Dependencies (Mac OSX)
---------------------------------

Cyclus archetype development is not fully supported on Mac.  Nonetheless,
because there are some use cases which require installation from source, we have
compiled a list of instructions that should be successful.  Use a Mac platform
at your own risk; we strongly recommend Linux as a primary development environment.

This guide assumes that the user has root access (to issue ``sudo`` commands) and
access to a package manager or has some other suitable method of automatically
installing established libraries. This process was tested using a fresh install
of 10.11.6 (El Capitan) using ``macports`` as the package
manager.  Macports installs packages in ``/opt/local``.  If installing to a
different location, (i.e. ``/usr/local``) change paths in the following
instructions accordingly. If you use ``homebrew``, try the following instructions
with brew commands in place of the port commands.

The command to install a dependency takes the form of:

.. code-block:: bash

  sudo port install package

where "package" is replaced by the correct package name. The minimal list of
required library package names is:

#. cmake
#. boost
#. hdf5
#. libxml2
#. libxmlxx2
#. sqlite3
#. doxygen
#. glibmm

Then install Coin-Cbc from source. They can be downloaded to any directory on
your computer:

**Coin-Cbc**: Download and build using the svn command in the terminal:

.. code-block:: bash

  svn co https://projects.coin-or.org/svn/Cbc/stable/2.8 Coin-Cbc
  cd Coin-Cbc/
  mkdir build
  cd build/
  ../configure --prefix=/opt/local
  make
  sudo make install


Finally, update your path and the following environment variables in your
``~/.profile`` (or ``~/.bashrc`` ) file:

.. code-block:: bash

  export DYLD_FALLBACK_LIBRARY_PATH=/opt/local/lib:/opt/local:$DYLD_FALLBACK_LIBRARY_PATH

  export CMAKE_MODULE_PATH=/opt/local/include:$CMAKE_MODULE_PATH
  export CMAKE_PREFIX_PATH=/opt/local:$CMAKE_PREFIX_PATH

  # add to PATH:
  export PATH=${HDF5_DIR}:/opt/local/bin:${HOME}/.local/bin:$PATH



.. _`Cyclus Homepage`: http://fuelcycle.org/
.. _`Cyclus User Guide`: http://fuelcycle.org/user/index.html
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cycamore Repo`: https://github.com/cyclus/cycamore


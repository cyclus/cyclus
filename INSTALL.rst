###########
Cyclus Core
###########

The core of the Cyclus nuclear fuel cycle simulator from the
University of Wisconsin - Madison is intended to be a simulation
framework upon which to develop innovative fuel cycle simulations.

To see user and developer documentation for this code, please visit
the `Cyclus Homepage`_.

*******************
Quick Start
*******************

If you want to get up and running as quickly as possible:

#. Install `VirtualBox <https://www.virtualbox.org/>`_
#. Download the `environment
   <http://cnergdata.engr.wisc.edu/cyclus/virtual-box/current/cyclus-conda-devs.ova>`_
#. Open the .ova file using VirtualBox (File->Import Appliance).
#. Login with the username ``ubuntu`` and pw ``reverse``
#. Skip to "Installing Cyclus"

Note that the Cyclus source code exists already on the virtual machine. You can
update it with

.. code-block:: console

    $ cd cyclus
	$ git pull origin cyclus

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

On some platforms, such as Ubuntu 14.04, the following are also necessary:

====================   ==================
Package                Minimum Version
====================   ==================
`g++`                  4.8.2
`libblas-dev`          1.2
`liblapack-dev`        3.5.0
====================   ==================


And a few optional dependencies:

====================   ==================
Package                Minimum Version
====================   ==================
doxygen (for docs)     1.7.6.1
tcmalloc (for speed)   any?
====================   ==================

*Note that the Debian/Ubuntu package `libtcmalloc` is NOT discovered correctly
by our build system.  Instead use `libgoogle-perftools-dev`.*

Installing Dependencies
=========================

Cyclus dependencies can either be installed via an operating system's package
manager or via Conda.

Via Conda
---------

*Note, at present, Conda dependency downloading is only supported for Linux*

A Conda installation may be the most straightforward for a new developer. These
instructions will be similar to those shown for Cyclus users `conda
installation <http://fuelcycle.org/user/install.html>`_.

#. Download the appropriate `miniconda installer <http://conda.pydata.org/miniconda.html>`_

#. Install miniconda

    .. code-block:: console

        $ bash Miniconda-3.5.2-Linux-x86_64.sh -b -p ~/miniconda

#. Add the following line to your ``.bashrc`` (``.bash_profile`` for Mac users):

    ``export PATH-"${HOME}/miniconda/bin:${PATH}"``

#. Install the dependencies

    .. code-block:: console

        $ conda install cyclus-deps

Installing Dependencies (Linux and Unix)
----------------------------------------

This guide assumes that the user has root access (to issue sudo commands) and
access to a package manager or has some other suitable method of automatically
installing established libraries. This process was tested using a fresh install
of Ubuntu 12.10 using apt-get as the package manager (scroll down further for
Mac OSX instructions).

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

For example, in order to install libxml++ (and libxml2) on your system, type:

.. code-block:: bash

  sudo apt-get install libxml++2.6-dev

If you'd prefer to copy/paste, the following line will install all *Cyclus*
dependencies:

.. code-block:: bash

   sudo apt-get install -y cmake make libboost-all-dev libxml2-dev libxml++2.6-dev libsqlite3-dev libhdf5-serial-dev libbz2-dev coinor-libcbc-dev coinor-libcoinutils-dev coinor-libosi-dev coinor-libclp-dev coinor-libcgl-dev libblas-dev liblapack-dev g++ libgoogle-perftools-dev

Boost Note
^^^^^^^^^^

The `libboost-all-dev` used above will install the entire Boost library, which
is not strictly needed. We currently depend on a small subset of the Boost
libraries:

#. libboost-program-options-dev
#. libboost-system-dev
#. libboost-filesystem-dev

However, it is possible (likely) that additional Boost libraries will be used
because they are an industry standard. Accordingly, we suggest simply installing
`libboost-all-dev` to limit any headaches due to possible dependency additions
in the future.

Installing Dependencies (Mac OSX)
----------------------------------------

Cyclus archetype development is not fully supported on Mac.  Nonetheless,
because there are some use cases which require installation from source,
we have compiled a list of instructions that should be successful. (Note that
the HDF5 interface is not working on Yosemite as of 1-Apr-2015. Ignore related
warnings when building cyclus). Use a Mac platform at your own risk, we strongly
recommend sticking to Linux for development.

This guide assumes that the user has root access (to issue sudo commands) and
access to a package manager or has some other suitable method of automatically
installing established libraries. This process was tested using a fresh install
of Yosemite 10.10.2 using macports as the package manager.  Macports installs
packages in /opt/local.  If installing to a different location,
(i.e. /usr/local) change paths in the following instructions accordingly. If
you use homebrew, try the following instructions with brew commands in place of
the port commands.

The command to install a dependency takes the form of:

.. code-block:: bash

  sudo port install package

where "package" is replaced by the correct package name. The minimal list of
required library package names is:

#. cmake
#. boost
#. libxml2
#. libxmlxx2
#. sqlite3
#. doxygen
#. glibmm

Then install Coin-Cbc and HDF5 from source. They can be downloaded to any
directory on your computer:
   
**Coin-Cbc**: Download and build using the svn command in the terminal:
   
.. code-block:: bash

  svn co https://projects.coin-or.org/svn/Cbc/stable/2.8 Coin-Cbc
  cd Coin-Cbc/
  mkdir build
  cd build/
  ../configure --prefix=/opt/local
  make
  sudo make install

**HDF5**: The 1.8.13 version appears to work better than 1.8.14.  Do not use
the macports distribution, it is definitely broken.
Download and build using the gzip Linux/Unix distribution of
`HDF5. <http://www.hdfgroup.org/ftp/HDF5/releases/hdf5-1.8.13/src/hdf5-1.8.13.tar.gz>`_   (For Safari users - the file will be automatically unzipped so change
the *mv* command in the the following codeblock to *mv hdf5-1.8.13.tar hdf5/* ).

.. code-block:: bash

  mkdir hdf5/
  mv hdf5-1.8.13.tar.gz hdf5/
  cd hdf5/
  mkdir build
  cd build/
  ../configure --prefix=/opt/local
  make
  sudo make install

Finally, update your path and the following environment variables in your
~/.profile (or ~/.bashrc ) file:

.. code-block:: bash

  export DYLD_FALLBACK_LIBRARY_PATH=/opt/local/lib:/opt/local:$DYLD_FALLBACK_LIBRARY_PATH

  export CMAKE_MODULE_PATH=/opt/local/include:$CMAKE_MODULE_PATH
  export CMAKE_PREFIX_PATH=/opt/local:$CMAKE_PREFIX_PATH

  export HDF5_DIR=/opt/local/hdf5/lib
  export HDF5_ROOT=/opt/local/hdf5

  # add to PATH:
  export PATH=${HDF5_DIR}:/opt/local/bin:${HOME}/.local/bin:$PATH


Installing Cyclus (Linux, Unix, and Mac OSX)
=============================================

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


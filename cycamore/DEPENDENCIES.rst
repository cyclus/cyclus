##############################################
Installing Cycamore Dependencies from Binaries
##############################################

To see user and developer documentation for this code, please visit
the `Cyclus Homepage`_.

This method describes two methods for installing Cycamore's only dependency,
the Cyclus Core, from binary distributions.  If you would like to install it
from source code, please see the `Cyclus Core repository
<http://github.com/cyclus/cyclus>`_.

.. contents:: Table of Contents
   :depth: 2

************
Dependencies
************

====================   ==================
Package                Minimum Version
====================   ==================
`Cyclus`               1.4
====================   ==================


***********************
Installing Dependencies
***********************

The Cyclus Core supports two binary installation options:

.. website_include_binary_start

#. `Conda`_ (Linux and Mac OSX)
#. `Debian package manager`_ (Linux only)


**Conda** is a cross-platform, user-space package manager aimed at simplifying
the installation of open source software. The Cyclus project uses Conda to
distribute pre-built Cyclus and Cycamore binaries.

The **Debian package manager** simplifies the installation of open-source
software. It contains all of the files required to use specific software, as
well as variety of relevant information: maintainer, description, version,
dependencies (other software or libraries required to use it).  The Cyclus
team provides pre-built Cyclus and Cycamore Debian packages to simplify
installation for the user. These packages are available for LTS Ubuntu version
14.04 and 16.04 (though they may also work on other Linux systems).

.. website_include_binary_end

.. website_include_conda_start

~~~~~~~~~~~~~~~~~~~~~
Conda
~~~~~~~~~~~~~~~~~~~~~


1. If you don't have Conda, start by installing the Python 3.x version of
   Anaconda_ (or miniconda_ for a more lightweight choice) to prepare it for
   Cyclus.

.. website_include_conda_end

2. Once you have Conda installed, installing Cyclus straightforward.

   .. code-block:: bash

      $ conda install -c conda-forge cyclus


.. website_include_deb_start

~~~~~~~~~~~~~~~~~~~~~~
Debian Package Manager
~~~~~~~~~~~~~~~~~~~~~~


This installation procedure assumes that you are using Ubuntu (LTS) 14.04 or
16.04. This method has only been tested on those Ubuntu versions. This
installation procedure also assumes that you have root access to you computer.

#. Install Cyclus dependencies:

   .. code-block:: bash 

     $ sudo apt-get install libtcmalloc-minimal4 libboost-filesystem-dev libboost-program-options-dev libboost-serialization-dev libhdf5-dev libxml++2.6-dev coinor-libcbc-dev
  
   WARNING: This dependency list is ONLY correct for the debian binary
   installation, additional dependencies are required to install from source.
   If you need/want more information about dependency installation please read the
   `dependency installation documentation`_.

#. Download the lastest version Cyclus Core Debian installation package:
   
   .. list-table::

      * - Ubuntu 14.04
        - `without Python 
          <http://dory.fuelcycle.org:4848/ubuntu/14.04/cyclus_latest.deb>`_ 
        - `with Python 2.7 
          <http://dory.fuelcycle.org:4848/ubuntu/14.04/python2/cyclus_latest.deb>`_ 
        - 

      * - Ubuntu 16.04
        - `without Python 
          <http://dory.fuelcycle.org:4848/ubuntu/16.04/cyclus_latest.deb>`_
        - `with Python 2.7 
          <http://dory.fuelcycle.org:4848/ubuntu/16.04/python2/cyclus_latest.deb>`_
        - `with Python 3.5 
          <http://dory.fuelcycle.org:4848/ubuntu/16.04/python3/cyclus_latest.deb>`_

   You can download previous/different version `here
   <http://dory.fuelcycle.org:4848/>`_.
    

        
#. Install the package by running:

   .. code-block:: bash 

     $ sudo dpkg -i cyclus_latest.deb

.. website_include_deb_end

.. _`Cyclus Homepage`: http://fuelcycle.org/
.. _`Cyclus User Guide`: http://fuelcycle.org/user/index.html
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cycamore Repo`: https://github.com/cyclus/cycamore
.. _Anaconda: https://www.continuum.io/downloads
.. _miniconda: http://conda.pydata.org/miniconda.html
.. _`dependency installation documentation`: https://github.com/cyclus/cyclus/blob/develop/DEPENDENCIES.rst

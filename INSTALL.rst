#########################
Cyclus Installation Guide
#########################

Cyclus uses the ``CMake`` cross-platform build-generator. To
simplify the usage of cmake to build Cyclus from source, a python script is
provided with the source.

.. contents:: Table of Contents

************
Dependencies
************

A full list of the Cyclus package dependencies is shown below:

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

************
Installation
************
.. website_include_start

Before going further with the installation procedure be sure you have installed
all the required dependencies listed above. We have provided detailed
instructions for `installing dependencies <DEPENDENCIES.rst>`_ for the major supported
systems.


Default Installation
--------------------

Run the install script:

.. code-block:: bash
  
  python install.py

If you successfully followed the instruction above, the cyclus binary has been
generated and placed in the  ``~/.local/`` in your home directory. 
You need to add ``~/.local/bin`` to the front of your ``$PATH``:

.. code-block:: bash
  
  echo 'export PATH="$HOME/.local/bin:$PATH' >> ~/.bashrc
  source ~/.bashrc

.. website_include_end

.. website_custom_start

Custom Cyclus Installation
--------------------------

The installation using the install script can be customized using the following
flags:

.. list-table::

  * - ``-h, --help``                              
    - show the help message and exit                                                                   

  * - ``--build_dir BUILD_DIR``                   
    - where to place the build directory                                                               

  * - ``--uninstall``                             
    - uninstall                                                                                        

  * - ``--no-update``                             
    - do not update the hash in version.cc                                                             

  * - ``--clean-build``                           
    - attempt to remove the build directory before building                                            

  * - ``-j THREADS, --threads``                   
    - THREADS the number of threads to use in the make step                                            

  * - ``--prefix PREFIX``                         
    - the relative path to the installation directory                                                  

  * - ``--config-only``                           
    - only configure the package, do not build or install                                              

  * - ``--build-only``                            
    - only build the package, do not install                                                           

  * - ``--test``                                  
    - run tests after  building                                                                        

  * - ``--coin_root COIN_ROOT``                   
    - the relative path to the Coin-OR libraries directory                                             

  * - ``--boost_root BOOST_ROOT``                 
    - the relative path to the Boost libraries directory                                               

  * - ``--hdf5_root HDF5_ROOT``                   
    - the path to the HDF5 libraries directory                                                         

  * - ``--cmake_prefix_path CMAKE_PREFIX_PATH``   
    - the cmake prefix path for use with FIND_PACKAGE, FIND_PATH, FIND_PROGRAM, or FIND_LIBRARY macros 

  * - ``--build_type BUILD_TYPE``                 
    - change the CMAKE_BUILD_TYPE                                                                      

  * - ``-D VAR``                                  
    - set environment variable(s).                                                                    


For example, if you have installed coin-Cbc from source or otherwise have it
installed in a non-standard location, you should make use of the ``coin_root``
installation flag. The otherwise identical process would look like:

.. code-block:: bash

    .../cyclus$  python install.py --coin_root=path/to/coin


CMake Cyclus Installation
-------------------------

If you are ``CMake`` aficionado you can also install Cyclus without using the
``install.py`` python script and use ``cmake`` directly, which should look like:


.. code-block:: bash

  mkdir bld
  cd build
  cmake .. -DCMAKE_INSTALL_PREFIX=~/.local/
  make
  make install

As usual you can customize your cmake installation using the proper cmake flag.
All cmake variables can be listed using: ``cmake -LAH``.
The main variables used are:

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

*************
Running Tests
*************

Installing Cyclus will also install a test driver (i.e., an executable of all of
our tests). We strongly recommend after a fresh install of Cyclus, or after
any modification on the source code, to rerun those tests in order to insure the
proper functioning of Cyclus. You can run the tests yourself via:

.. code-block:: bash

    $ cyclus_unit_tests


.. _`Cyclus Homepage`: http://fuelcycle.org/
.. _`Cyclus User Guide`: http://fuelcycle.org/user/index.html
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cycamore Repo`: https://github.com/cyclus/cycamore
.. _`for installing those dependencies for the major supported systems`: DEPENDENCIES.rst

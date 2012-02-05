
.. summary Information on getting Cyclus from the repository and building it on a new system

Building Cyclus
===============

The `Cyclus Homepage`_ has much more detailed guides and information.
This Readme is intended to be a quick reference for building cyclus for the
first time.

Dependencies
------------

The Cyclus code requires the following software and libraries.

====================   ==================
Package                Minimum Version   
====================   ==================
`CMake`                2.8                 
`HDF5`                 1.8.3           
`libxml2`              2                 
`boost`                1.34.1            
`lapack`               3.4.0             
`trilinos (teuchos)`   10.8.4            
====================   ==================

An overview of some more complicated package builds and installations (e.g.
lapack, teuchos, etc.) can be found at the `Cyclus Homepage`_

Build System
------------

In order to facilitate future compatibility with multiple platforms, Cyclus is
built using  `Cmake <http://www.cmake.org>`_. This relies on CMake version
2.6 or higher and the CMakeLists.txt file in `src/`. It is
recommended that you use CMake to build the Cyclus executable external to the
source code. To do this, execute the following steps::

    .../core/$ mkdir build
    .../core/$ cd build
    .../core/build$ cmake ../src

You should see output like this::

    ...
    ...
    >> -- Configuring done
    >> -- Generating done
    >> -- Build files have been written to: /core/build
    /core/build$ make cyclus
    >> Scanning dependencies of target cyclus
    ...
    ...
    >> [100%] Building CXX object CMakeFiles/cyclus.dir/SourceFac.cpp.o
    >> Linking CXX executable cyclus
    >> [100%] Built target cyclus

Now, you can make cyclus, and run it with some input file, for this example, call it input.xml::

    .../core/build$ make
    .../core/build$ ./cyclus input.xml

The `Cyclus Homepage`_ has much more detailed guides and information.  If
you intend to develop for *Cyclus*, please visit it to learn more.


.. _`Cyclus Homepage`: http://cyclus.github.com



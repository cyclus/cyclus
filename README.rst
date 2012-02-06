
_______________________________________________________________________
Cyclus Core
_______________________________________________________________________

**Last Updated: 2.6.2012**

The core of the Cyclus nuclear fuel cycle simulator from the University of 
Wisconsin - Madison is intended to be a simulation framework upon which to 
develop innovative fuel cycle simulations. 

To see user and developer documentation for this code, please visit the `Cyclus Homepage`_.


-----------------------------------------------------------------------
LISCENSE
-----------------------------------------------------------------------

::

    Copyright (c) 2010-2012, University of Wisconsin Computational Nuclear Engineering Research Group
     All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    
      - Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.
      
      - Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      
      - Neither the name of the University of Wisconsin Computational
        Nuclear Engineering Research Group nor the names of its
        contributors may be used to endorse or promote products derived
        from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

------------------------------------------------------------------
Building Cyclus
------------------------------------------------------------------

The `Cyclus Homepage`_ has much more detailed guides and information.
This Readme is intended to be a quick reference for building cyclus for the
first time.

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

~~~~~~~~~~~~~~~~~~~~~~~~~~~
Building and Running Cyclus
~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
    >> -- Build files have been written to: .../core/build
    /core/build$ make cyclus
    >> Scanning dependencies of target cyclus
    ...
    ...
    >> [100%] Building CXX object CMakeFiles/cyclus.dir/SourceFac.cpp.o
    >> Linking CXX executable cyclus
    >> [100%] Built target cyclus

Now, you can make cyclus, and run it with some input file, for this example, call it `input.xml`::

    .../core/build$ make
    .../core/build$ ./cyclus input.xml

The `Cyclus Homepage`_ has much more detailed guides and information.  If
you intend to develop for *Cyclus*, please visit it to learn more.


.. _`Cyclus Homepage`: http://cyclus.github.com



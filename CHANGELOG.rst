=================
cyclus Change Log
=================

.. current developments

v1.5.3
====================

**Added:**

* New ``CYCLUS_PLATFORM`` string for uniquely identifying the
  platform.  This is initialized with the ``cyclus_platform()``
  macro.
* New ``download_platform()`` macro for downloading a tar.gz
  file and its platform-specific equivalent.


**Changed:**

None

* Cyclus agent libraries will now have the the install directroy and
  rpath correctly set to when using the ``install_agent_lib()`` in CMake.
* Updated how fast compiling downloads and searches for files so that
  only platform-specific files are grabbed.


**Deprecated:**

* CMake policies CMP0040 and CMP0042 supressed.


**Fixed:**

* Fixed issue with ``cyclus_unit_tests`` not being able to find
  the agents library in linking on macOS.
* Fixed SQLite backend bug with Boost v1.66.
* CMake RPATH now correctly uses semicolon delimiters, rather
  than colons.
* ``DynamicLoadingTests`` now properly start/stop Python.




v1.5.2
====================

**Changed:**

* Unit tests now link to libcyclus and agents, rather than recompiling all the sources
  into the test executable.


**Fixed:**

* Fixed issue with Python agent state vars that were set in the input file
  instead using the default value set on the archetype for optional state vars.
* Fixed issue with Python agent prototypes not being registered correctly when created.
  This caused segfaults due the the agents being deallocated too soon.
* Fixed many issues with institution kinds checking "Institution", rather than "Inst".





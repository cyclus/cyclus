**Added:**

* New ``CYCLUS_PLATFORM`` string for uniquely identifying the
  platform.  This is initialized with the ``cyclus_platform()``
  macro.
* New ``download_platform()`` macro for downloading a tar.gz
  file and its platform-specific equivalent.

**Changed:** None

* Cyclus agent libraries will now have the the install directroy and
  rpath correctly set to when using the ``install_agent_lib()`` in CMake.
* Updated how fast compiling downloads and searches for files so that
  only platform-specific files are grabbed.

**Deprecated:**

* CMake policies CMP0040 and CMP0042 supressed.

**Removed:** None

**Fixed:**

* Fixed issue with ``cyclus_unit_tests`` not being able to find
  the agents library in linking on macOS.
* Fixed SQLite backend bug with Boost v1.66.
* CMake RPATH now correctly uses semicolon delimiters, rather
  than colons.

**Security:** None

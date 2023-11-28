=================
cyclus Change Log
=================

Since last release
====================

**Added:**

* adding compatibility with cbc 2.10 (#1557)
* In `src/pyne.h`, replace a macro `isnan()` with `using std::isnan`, 
  as the former has led to undesired macro expansion in Boost header files. (#1560)
* updated cyclus_nuc_data.h5 URL after Dory retirement (#1567)
* now default quantize are zero which corresponds to no quantize policy. (#1552)
* Added several lines to DEPENDENCIES.rst to explain that even when using a 
  machine with a single python3 install (such as a fresh Ubuntu 20.04), install 
  will fail unless update-alternatives has been used to point python at the 
  correct python3 version (#1558)
* build and test are now fown on githubAction in place or CircleCI (#1569)
* Have separate workflows for testing, publishing dependency images, and publishing release images (#1597, #1602, #1606, #1609, #1629)
* Add Ubuntu 20.04 to the list of supported platforms (#1605, #1608)
* Add random number generator (Mersenne Twister 19937, from boost) and the ability to set the seed in the simulation control block (#1599)
* Adds active and dormant buying cycles in buy policy (#1596)
* Add random number generator (Mersenne Twister 19937, from boost) and the ability to set the seed in the simulation control block (#1599)

**Changed:**

* Moved to unified CHANGELOG Entry and check them with GithubAction (#1571)
* Major update and modernization of build (#1587)
* Changed Json formatting for compatibility with current python standards (#1587)
* Changed README.rst installation instructions, tested on fresh Ubuntu-22.04 system with Python 3.11 (#1617)

**Removed:**

* A duplicate `using std::list` in `src/hdf5_back.cc.in`, which triggers compiler 
  errors with some GCC versions. (#1560)

**Fixed:**

* Issue #1312. (#1560)
* fix sell_policy that was offering bids when capacity was inbetween 0 and the
  quantize, bids that one was not able to fullfill and caused cyclus to crash. (#1552)
* Resolve deprecation warnings involving <boost/detail/sp_typeinfo.hpp> (#1611)
* Resolve segmentation faults when calling Cbc (#1614)



v1.5.5
====================

**Added:**

* Added a test example of Cyclus sensitivity analysis studies conducted using Dakota.
* Script and CI implementation ensuring at least 1 news file have been recreated.
* Added capability to register unit when reporting values in a time series.
* Added `map<string,map<string,double>>` typesystem. This can be used with C++ archetypes. But not python archetypes.


**Changed:**

* git and open-ssh added to the dockerfile (removed from CI).
* MACOS only: link against PYTHON lib in the binary building process. It should not be required when building the Python package as python knows about itself. For some reason, this is only true (and working) on MACOS, and was an issue when using Conda to install Python and other Cyclus deps.
* Updated the Cyclus quick installation instructions to reflect our
use of conda to install dependencies.


**Removed:**

* git and open-ssh from CI (added to the dockerfile).
* Reference to `develop` branch tests in the readme file.


**Fixed:**

* News now checks diff against cyclus/master not origin/master.
* News check now triggers on PRs only.
* Allows build against Python => 3.7.
* Change cyclus-deps Docekrfile accordingly. (changing cython version to 0.28.5).




v1.5.4
====================

**Added:**

* Python bindings to the ``Position`` class.
* Input validation for Latitude and Longitude
* New `Mocksim::DummyProto` where capacity and commodity of the prototype can be defined.
* The Python typesystem generator has been extended to include
  types that should be passed by their C++ shared pointers, such
  as resources. ``Materials`` and ``Products`` have thus been added as
  fully valid types on their own in the typesystem.  Note that since
  resources already have classes of their own in the typesystem, state
  variables for these classes are ``MaterialStateVar`` and ``ProductStateVar``.
* Added the Decision Phase. This phase occurs after the tock phase and allows
  cyclus agents to make decisions based on what has occured during the
  tick and tock phase of the current timestep. 


**Changed:**

* `cycpp.py` now takes advantage of the `$CPP` environment variable if defined,
  falls back on `cpp` if not defined
* `cyclus_deps` docker container built now on debian9, removed gcc from apt-get
  package, build the full list of conda package (don't rely on cyclus-deps conda
  package as it is temporary broken), define CXX CC and CPP environment variable
* `cython` compatibility limited to >=0.25 and <0.27
* ``dbtypes_gen.py`` will now throw approriate errors for incorrect
  JSON in ``queryable_backend.h``.
* Python interface will now raise exceptions when it encounters an error in C++ or
  Cython.
* Updated cycstub readme to reflect changes
* Updated the record time series function to also include the name of the field being
  recorded. 



**Deprecated:**


**Fixed:**

* `boost/uuid/sha1.hpp` include now varies according to the Boost version
  installed due to its deprecation in boost > 1.67
* Python inventories required a value for ``capacity`` even though it was optional. This
  change also enables ``None`` valued capacities, which are treated as infinite.
* Fixed issue where resource buffer Python bidnings would attempt to append to
  the resources themselves when popping resources out of the buffers.
* Fixed issue when pushing many resources to a resource buffer would attempt to
  append the entire buffer
* Error in generation of ``dbtypes.json`` file led to incorrect
  enumeration of of some database types. This in turn led to segfaults
  when using these types in the Python typesystem. It also made the HDF5
  backend generator skip some types.
* Invalid JSON in ``queryable_backend.h`` has been fixed.
* Fixed cycstub build issue which failed to find Cyclus shared libraries
* Fixed cycstub unit test issue that failed to locate stub library shared object




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





**Added:** None

**Changed:** None
- `cycpp.py` now takes advantage of the `$CPP` environment variable if defined,
  falls back on `cpp` if not defined
- `cyclus_deps` docker container built now on debian9, removed gcc from apt-get
  package, build the full list of conda package (don't rely on cyclus-deps conda
  package as it is temporary broken), define CXX CC and CPP environment variable
- `cython` compatibility limited to >=0.25 and <0.27

**Deprecated:** None

**Removed:** None

**Fixed:** None
- `boost/uuid/sha1.hpp` include now varies according to the Boost version
  installed du to its deprecation in boost > 1.67

**Security:** None

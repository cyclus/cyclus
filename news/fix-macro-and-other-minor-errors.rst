**Added:** None

**Changed:**
- In `src/pyne.h`, replace a macro `isnan()` with `using std::isnan`, as the former has led to undesired macro expansion in Boost header files.

**Deprecated:** None

**Removed:**
- A duplicate `using std::list` in `src/hdf5_back.cc.in`, which triggers compiler errors with some GCC versions.

**Fixed:** 
- Issue #1312

**Security:** None

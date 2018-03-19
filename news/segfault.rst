**Added:**

* The Python typesystem generator has been extended to include
  types that should be passed by their C++ shared pointers, such
  as resources. ``Materials`` and ``Products`` have thus been added as
  fully valid types on their own in the typesystem.  Note that since
  resources already have classes of their own in the typesystem, state
  variables for these classes are ``MaterialStateVar`` and ``ProductStateVar``.

**Changed:**

* ``dbtypes_gen.py`` will now throw approriate errors for incorrect
  JSON in ``queryable_backend.h``.

**Deprecated:** None

**Removed:** None

**Fixed:**

* Error in generation of ``dbtypes.json`` file led to incorrect
  enumeration of of some database types. This in turn led to segfaults
  when using these types in the Python typesystem. It also made the HDF5
  backend generator skip some types.
* Invalid JSON in ``queryable_backend.h`` has been fixed.

**Security:** None

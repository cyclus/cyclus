=================
cyclus Change Log
=================

.. current developments

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





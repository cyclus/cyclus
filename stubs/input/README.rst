***********************************************************************
Cyclus Stub Model Input Examples
***********************************************************************
This directory includes working input files for running simulations with
the Cyclus nuclear fuel cycle simulator.

To see user and developer documentation for the cyclus code, please visit
the Cyclus Homepage at http://fuelcycle.org/.


------------------------------------------------------------------
How To Use These Input Files
------------------------------------------------------------------
Let's say you've decided to implement a new Facility model. Let's say it's a
one group burnup approximation of some kind. You want to call it OneGroupReactor.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Modifying the Inputs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
If your model is based on a StubFacility, for example, you may want to search
and replace instances of StubFacility with OneGroupReactor. If you've altered
the StubFacility rng schema to initialize your module, you'll need to alter the
input xml. Else, you'll just need to search and replace. Try running these inputs
either with the run_inputs script in the bin directory or on their own with
cyclus <infile>.

Cycamore Sample Inputs: Minimal Input
===============================================

This group of input files are designed to demonstrate the smallest possible
system that engages all of Cyclus' core capabilities.

Single Source <--> Single Sink
-------------------------------

This example uses a single source facility and a single sink facility, both
operating in the same region and institution.  In each time step, the sink
facility will issue a request for bids for its full capacity, the source will
make a bid to provide that much material, and the material will be traded.

Although these archetypes allow for the specification of a capacity, this
simulation relies on deafult capacities in both the source and sink, allowing

Understanding the input file
+++++++++++++++++++++++++++++

`control` block
~~~~~~~~~~~~~~~
This simulation starts in January 2000 and proceeds for 10 months.

`archetypes` block
~~~~~~~~~~~~~~~~~~
This simulation uses four archetypes:

* the `Source` archetype from the `cycamore` library
* the `Sink` archetype from the `cycamore` library
* the `NullRegion` archetype from the Cyclus core `agents` library
* the `NullInst` archetype from the Cyclus core `agents` library

`facility` block
~~~~~~~~~~~~~~~~~
This simulation defines two facility prototypes:

* a prototype named `SomeSink` based on the `Sink` archetype, that defines a
  single output commodity named `commodity`
* a prototype named `SomeSource` based on the `Source` archetype, that defines a
  single input commodity named `commodity`

Note that both of these prototypes use the default capacity of 10\ :sup:`299`
(effectively infinite).

`region` block
~~~~~~~~~~~~~~
This simulation defines only one region based on the `NullRegion` archetype,
and named `SingleRegion`.  This archetype has no configuration other than a
single institution.

`institution` block
~~~~~~~~~~~~~~~~~~~
This simulation defines only one region based on the `NullInst` archetype, and
named `SingleInstitution`.  All institutions allow an `initialfacilitylist`,
but otherwise this archetype has no configuration.

`initialfacilitylist` block
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this simulation, the only institution has a list of two initial facilities.
One of these facilities is based on the `SomeSink` prototype and the other is
based on the `SomeSource` prototype.

`recipe` block
~~~~~~~~~~~~~~~
This simulation defines a single material recipe called `commod_recipe` made
up of 100% of H-1 (010010000).


.. summary Some developers notes on how resources work

Resources In Cyclus
=================================

Introduction
------------

The following section will discuss the Resource class, the base class for items 
that are passed between agents in a Cyclus simulation.

The Resource class is the parent class of the material class in *Cyclus*. A 
resource must have knowlege of what is and how it was created.  Accordingly, it 
keeps track of its units, quality, quantity, the id of its creator, and if it 
was spawned from the splitting of a different resource. 
    
The Resource class keeps track of two Tables in the Cyclus output 
database: Resource Types and Resources. For each new type of resource 
introduced in the simulation, a row is added to the Resource Types 
Table. When a new resource is created in the simulation, a row is 
added to the Resources table. 

Materials are the primary Resource that is transacted in a *Cyclus* simulation.  
Conceptually, though, a resource can be anything that might be an interesting 
traded item (e.g., electricity, money, or workers).


Quantity
---------

A resource object must have a quantity associated with it, expressed in its 
native unit. The native unit for material quantity is kilograms, the SI unit of 
mass. However, a developer could imagine that an electricity resource might have 
units of kW, MW, or GW and a money resource might have units of dollars, euros, 
or yuan. 

Quality
---------

The quality of a resource should be captured by a set of attributes. For 
example, a Material's primary quality attribute is its composition. A quality 
attribute of a worker might be their skillset. These quality attributes are 
intended to be used to filter the acceptability of offers of some resource to 
fulfill requests of some other resource.


Resource Data
--------------

Data contained by all resources includes : 

 * Units as discussed above.
 * A quantity, as discussed above.
 * An ID, keeping it distinct from other resources.
 * An originalID, which allows its history to be traced back through the 
   original resource it once was (if it was separated from the original resource 
   at some point).


Resource Methods
-----------------

The key method implemented by all resources returns a boolean to be used by the 
market for comparison of two commodities. The quality of two resources can be 
compared in order to determine whether the offer resource sufficiently satisfies the 
request resource. ::

    request_rsrc->checkQuality(offer_rsrc)

The boolean that it returns is used by the market to match requests with offers.

Resources
~~~~~~~~~

Resources are simulation objects that can or should be traded and tracked 
between facilities, institutions, and regions. Examples of resource types of 
interest in fuel cycle analysis use cases include people, money, electricity, 
building materials, or fuel materials. These resources are represented in the 
simulation as discrete objects. Their transport and characteristics over time 
are recorded in Resource tables by the simulation. 

To serve use cases in which economics is a driver or important fuel cycle 
analysis metric, resource types such as money, electricity, building materials, 
and workers will be essential to transport, track, and conserve. 

For use cases in which environmental and health metrics are of interest, the 
transport, tracking, and conservation of fuel materials is paramount. 

Generic Characteristics 
------------------------

All resources have an amount and a standard unit. For example, a Material 
resource object may have an amount of 100 in units of metric tons. Similarly, a 
Workforce resource may have a quantity of 20 in units of people and a Mony 
resource object may have an amount 1,000 in units of Euros.

All resources must also have a commodity associated with them. A resource's 
commodity is a mutable characteristic. That is, a Material 
resource may change from one commodity to another over time (that is, a material
object may be fresh fuel at the beginning of the simulation and may be spent 
fuel 18 months later). 

Since resources may be split into two or combined with other resources, the 
resource amount is mutable as well.

Specific Characteristics
------------------------

In addition to the generic characteristics that all resources must have, 
specific resource types can be defined by the developer to have arbitrarily many 
defining characteristics.  These specific characteristics are necessary for 
comparing and trading resources during the simulation and for analyzing fuel 
cycle metrics at the conclusion of the simulation. 

For example, a Workforce resource may have specific characteristics such as the 
skill set of the workers, the number of hours a day they can work, and the 
amount that they expect to be paid.  The generic amount and unit information 
will be stored for all resources, whereas the specific Workforce resource 
defining characteristics will be stored only for the Workforce resources in the 
simulation. 

Each of these categories should constitute its own resource class in Cyclus. A 
single Resource Type (e.g. people) might have many sub-types which may not be 
interchangeable. These subtypes will be dependent on quality and quantity checks 
provided for the class.


Resource Transport
------------------



Resource Conservation
---------------------

Most resources must be conserved. Material must always be accounted for, capital 
does not evaporate into thin air, etc. To obey laws of physics and economics, 
the simulation framework provides a conservative interface for absorbing and 
extracting resources from one another (note, this should probably be in future 
tense... it's true in Materials, but not so much for Resources at large).

Some resources do not need to be conserved exactly however. A workforce, for 
example, may ebb and flow in size, since human careers are not constrained by 
any conservation laws (two workers can become three, one worker might decide to 
be a baker, etc.). For resources that need not be conserved, the developer is 
not constrained to the use of the conservative absorb and extract methods.

Resource Tracking
------------------

The generic and specific characteristics of each resource object over time is 
tracked by the simulation and recorded in the output database. Entries in the 
database are **always** written when 

* some characteristic of the resource changes or
* its location (owning facility, institution, or region) changes.


Entries in the database **may** be written any time a facility or resource 
developer creates a trigger for such an event. 


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


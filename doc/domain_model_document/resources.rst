
.. summary Some developers notes on how Resources work

Resources 
==========

Introduction
------------

The following section will discuss the concept of Resources in Cyclus. Resources 
are objects that are passed between agents in a Cyclus simulation.

Materials are the primary Resource that is transacted in a *Cyclus* simulation.  
Conceptually, though, a Resource can be anything that might be an interesting 
traded item (e.g., electricity, money, or workers).

In support of  metrics calculations, a Resource must have knowlege of what is 
and how it was created.  Accordingly, it keeps track of its units, quality, 
quantity, the id of its creator, and whether it was spawned from the splitting of a 
different Resource. 
    
Resources are simulation objects that can or should be traded and tracked 
between facilities, institutions, and regions. Examples of Resource types of 
interest in fuel cycle analysis use cases include people, money, electricity, 
or Materials. These Resources are represented in the 
simulation as discrete objects. Their transport and characteristics over time 
are recorded in Resource tables by the simulation. 

To serve use cases in which economics is a driver or important fuel cycle 
analysis metric, Resource types such as money, electricity, Materials, 
and workers will be essential to transport, track, and conserve. 

For use cases in which environmental and health metrics are of interest, the 
transport, tracking, and conservation of fuel Materials is paramount. 

Attributes of all Resources include : 

 * A **Quality** defined by specific characteristics of its Resource type.
 * A **Quantity** in a native **Unit**
 * An ID, keeping it distinct from other Resources.
 * An originalID, which allows its history to be traced back through the 
   original Resource it once was (if it was separated from the original Resource 
   at some point).

.. _resource-quantity:

Quantity
---------

A Resource object must have a quantity associated with it, expressed in its 
native unit. The native unit for Material quantity is kilograms, the SI unit of 
mass. However, a developer could imagine that an electricity Resource might have 
units of kW, MW, or GW and a money Resource might have units of dollars, euros, 
or yuan. 

Each Resource object, then, has a total quantity in its native unit. The electricity 
offered by the DC Cook Unit 1 Reactor, for example, is 1,048 MW. 
Since Resources may be split into two or combined with other Resources, the 
Resource amount is mutable over time.

.. _resource-quality:

Quality
---------

In addition to the product-quantity_ characteristics that all Resources must have, 
specific Resource types can be defined by the developer to have arbitrarily many 
defining characteristics. These specific characteristics are necessary for 
comparing and trading Resources during the simulation and for analyzing fuel 
cycle metrics at the conclusion of the simulation. 

The quality of a Resource is captured by this set of attributes. For example, a 
Material's primary quality attribute is its isotopic composition. Quality 
attributes of a worker, on the other hand, might be their skillset, the number 
of hours a day that they can work, and they amount that they expect to be paid. 

The generic amount and unit information will be stored for all Resources, 
whereas the specific Workforce Resource defining characteristics will be stored 
only for the Workforce Resources in the simulation. 

All of these quality attributes, both generic and specific, are intended to 
facilitate differentiation of Resources from one another in the matching. 

That is, a single Resource Type (e.g., Workers) might have many sub-types which 
may not be interchangeable (e.g., reactor operators vs. crane operators). The 
interchangeability of these subtypes will be dependent on quality and quantity 
checks provided for the class.


Resource Methods
-----------------

The key method implemented by all Resources returns a boolean to be used by the 
market for comparison of two commodities. Various quality aspects of two Resources can be 
compared in order to determine whether the offer Resource sufficiently satisfies the 
request Resource. ::

    request_rsrc->checkAttribute(offer_rsrc)

The information that such a member function will return is used by the Agents to 
generate preferences about accepting or rejecting a Bid. 


Resource Conservation
---------------------

Most Resources must be conserved. Material must always be accounted for, capital 
does not evaporate into thin air, etc. To obey laws of physics and economics, 
the simulation framework will provide a conservation utility, which provides an 
interface for absorbing and extracting Resources from one another 
conservatively. 

However, some Resources do not need to be conserved exactly. A workforce, for 
example, may ebb and flow in size, since human careers are not constrained by 
any conservation laws (two workers can become three, one worker might decide to 
retire or become a baker, etc.). For Resources that need not be conserved, the 
developer is not constrained to the use of the conservative absorb and extract 
interface methods.

Resource Tracking
------------------

The generic and specific characteristics of each Resource object over time are 
tracked by the simulation and recorded in the output database. Entries in the 
database are **always** written when 

* some characteristic of the Resource changes or
* its location (owning facility, institution, or region) changes.

Entries in the database **may** be written any time a facility or Resource 
developer creates a trigger for such an event. 

The Resource class keeps track of two Tables in the Cyclus output 
database: Resource Types and Resources. For each new type of Resource 
introduced in the simulation, a row is added to the Resource Types 
Table. Thus, when a new Resource is created in the simulation, a row is 
added to the Resources table. 


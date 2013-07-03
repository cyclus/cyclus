
Simulation Environment Overview
===============================

Agents, Resources, and Commodity Exchanges
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In the most basic sense, Cyclus provides a platform for
agent-based simulation that steps through time.  During each time
step:

  * Supplier agents will make bids to provide resources in response to
    specific requests from consumer agents.
  * Consumers will assess the bids and sort them according to their
    own assessment of preference.
  * Commodity exchanges will collect the consumer preferences, match
    them with suppliers in an optimal way, and order suppliers to ship
    resources to consumers.

All consumer and supplier agents are :doc:`Facility Agents
<facility>`, the most common and fundamental type of agent in the
simulation.  Cyclus also provides a notion of facility ownership by
:doc:`Institution Agents <institution>`, and geopolitical location by
:doc:`Region Agents <region>`.  In general, :doc:`Institution Agents
<institution>` and :doc:`Region Agents <region>` have the ability to
influence the bid-making and preference setting behavior of the
Facility agents to which they are related.

Flexibility is achieved by allowing users/developers to swap out
implementations of different agents, thereby changing the way they
interact with other agents, changing the way they model the physical
processes that affect their own inventories, or both.

Core Simulation Objects
~~~~~~~~~~~~~~~~~~~~~~~~~~~

There are five core simulation objects:

 1. :doc:`Timer`: Manages the progression through time and invokes different
    phases within each time step.
 2. :doc:`Markets`: Implement a matching algorithm across a set of suppliers
    and consumers of a given commodity.
 3. :doc:`Agents`: Interact with each other by issuing requests for resources
    of bids to provide resources, and by modeling physical processes
    that change those resources.
 4. :doc:`Resources`: Describe the quantity and quality of a good to be
    traded among agents in the system.
 5. :doc:`Messages`: Flow among agents to describe requests, bids,
    preferences and orders.

Cyclus Utilities
~~~~~~~~~~~~~~~~~

A number of utilities provide services for support of common simulation needs:

 * XMLQueryEngine: facilitate parsing of input 
 * BookKeeper: facilitate recording output to a database
 * CycException: error handling
 * Logger: interactive user feedback



Cyclus Module Development Kit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 * DecayHandler (? where should this be?)


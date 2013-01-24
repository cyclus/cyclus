
Timer
=====

The Timer in Cyclus is the mechanism by which simulation time
progresses. Each time period is separated into a discrete number of
steps and phases, where each subsequent phase is guaranteed to begin
only after the preceding phase has ended. This allows entities in the
simulation a guarantee of what kinds of actions have already been
taken or will be taken in the future. The steps and phases are ordered
in such a way that makes logical sense with respect to dynamic supply
chain management. Most importantly, the phases occur **concurrently**,
i.e., the simulation entities enter and exit the phase in an unordered
manner. Only the steps and phases themselves are ordered.

Overview
========

The Timer's process can be nominally be broken into three steps:

 * pre-resource exchange
 * resource exchange
 * post-resource exchange

Each step can be broken up into phases, and new phases may be added
as needed in the development cycle. Of note, though, is that phases
are independent of one another, i.e., they comprise an encapsulated
moment of time. Therefore, the addition of new phases makes no
impact on already-existing phases, allowing pre-existing modules to be
used in an updated simulation environment with additional phases.
Another way to think of this capability is that any old modules simply
do not act in additional phases.

Pre-Resource Exchange Step
==========================

Prior to the resource exchange, modules are allowed to act. This is
effectively the beginning of the time step. There are currently only
two phases: Building and Execution.

Building Phase
--------------

There are entities within the Cyclus environment that are tasked with
"building" other entities. For example, a ReactorBuilder may be tasked
with building reactors if electricity demand exceeds electricity
supply. In the building phase, any such entities are tapped to execute
their building instructions. Importantly, building decisions made
during this phase must be independent of any other building decisions
made by other entities during this phase.

Execution Phase
---------------

The execution phase allows entities in Cyclus to perform any actions
they deem necessary prior to the resource exchange step. For example,
a Reactor may wish to move fuel around various buffers. Many of these
actions could be coupled into the first phase of the resource exchange
step, but the development team offers this separation in order to
provide a temporally separate mechanism in which to perform these
actions. That is, entities can be guaranteed that these actions occur
before any other entities enter the resource exchange step.

Resource Exchange Step
======================

The resource exchange step is the foundation around which the Cyclus
simulation engine executes. Conceptually it is simply a network
routing problem where entities in the simulation either produce or
consume commodities are matched to meet consumption requirements at
the lowest "cost". In practice, though, it is much more complicated
due to the high number of constituents (i.e. isotopes) that must be
specified by consumers and producers. In addition,
constituent-specific constraints may be required. Accordingly, we
employ a relatively complicated system that include requests for
proposals or bids, bids, and preference assignments of those
bids. Additionally, we allow institution and regional entities to
alter certain aspects of this process by imposing policy constraints
and pricing mechanisms. The general process is comprised of the
following phases:

 * Request for Proposals/Bids (consumers)
 * Provide Bids (producers)
 * Assign Preferences (consumers)
 * Add Policy Constraints/Costs (policy-makers)
 * Match Bids with Preferences (matching algorithm)
 * Send/Receive Orders (producers/consumers)

Request for Proposals/Bids Phase
--------------------------------

In this phase, any entity which is need of procuring a commodity posts
the procurement specification to the exchange. A specification
includes:

  * the required amount of the commodity
  * details on the make-up of the commodity
  * whether the procurement can be satisfied by more than one producer

All procurement specifications are collected from entities during this
phase. The phase ends when all relevant entities have been queried.

Provide Bids Phase
------------------

In this phase, any entity which produces commodities is allowed to bid
on the any proposal in the set of proposal requests that corresponds
to the commodity(ies) produced by the producer. A bid includes:

  * a proposed matching specification to the request for bids
  * an amount of the commodity
  * a price to meet this specification

In addition, bidders can provide a constraint on their production that
spans all bids. To clarify we provide an example.

      An EnrichmentFacility provides enrichment services using the
      base unit of SWUs. Let us assume that the tails fraction of the
      enrichment process is 0.3, the natural enrichment feed is 0.7,
      and the total SWUs that can be used is 500.
      
      Further let us assume that there are two customers. One requests
      100 kg of 3.5 w/o Uranium. The other requests 50 kg of 4.5 w/o
      Uranium.

      The EnrichmentFacility could match either request. The first
      requires 316 SWUs while the second takes 440. Accordingly, the
      EnrichmentFacility places bids for each request, but adds a
      constraint that the combination of all bids must be less than or
      equal to 500 SWUs. 

All production bids are collected from entities during this phase. The
phase ends when all relevant entities have been queried.

Assign Preferences Phase
------------------------

During this phase, consumers respond to the bids that have been placed
on their proposals. They are allowed to assess each bid in the set of
bids and assign a preference to them. Preference selections use a
cardinal ordering [Strotz]_, i.e. their relative numerical values
impart a notion of *degree of preference*. These numbers can be
thought of strictly as economic costs or as such costs combined with
externalitites.

Add Policy Constraints/Costs Phase
----------------------------------

The policy addition phase of the resource exchange step allows actors
at the institutional and regional levels affect the exchange.
Institutions are allowed to alter costs between actors within their
organizational boundaries, e.g. if a consumer of enriched uranium has
no preference between two suppliers, its institution can add
incentives to use a supplier within the same organization. This is one
possible way contracts can be represented between institutions.

Regional actors are also allowed to affect the resource exchange
process. Regions can place constraints on material entering or leaving
their boundaries. For example, a region may place a restriction on any
outgoing enriched uranium that it must be low enriched (i.e. <= 20 w/o
U-235). Similarly, regions may affect the preference of a given
transaction by increasing its cost. This is one way in which tax
structures can be modeled in the simulation.

Match Bids with Preferences Phase
---------------------------------

During this phase, all information is known about all possible
transactions. A transport network linear program (LP) is solved to
determine the appropriate resource flows. The answer is binding, i.e.,
actors are not allowed to alter the flows once they have been
determined. In other words, the verdict has been passed, and they
should have spoken before and now must hold their peace.

Note that future modifications may alter the internal nature of this
process. An easy supposition is that in the future there may be a need
for integer decision variables in the network flow, transforming the
LP into a mixed integer-linear program (MILP). This is an ongoing area
of research.

Send/Receive Orders Phase
-------------------------

The transactions that were selected in the prior phase are now
executed. Producers of a commodity are tapped to send the resources on
which they bid. 

It should be noted that the resources sent may not match precisely the
resource specification offered in the bidding phase. Take, for
example, a reprocessing facility that bid on a set of proposals. The
agent may have provided a solution given that it might win every
proposal. If it does not, its provided solution may differ slightly or
largely from the specification it provided. We leave it to the
developers of the supplier agents to deal with orders received that
differ from their specifications. One possible solution mechanism is
to alter preferences in future time periods in order to penalize
suppliers that do not meet their specifications.

Post Resource Exchange Step
===========================

After the resource exchange, modules are allowed to act. This is
effectively the end of the time step. There are currently only
two phases: Decommissionin and Execution.

Decommissioning Phase
---------------------

Facilities in Cyclus are allowed to have limited lifetimes. If the
current time step corresponds with a facility's lifetime it will
invoke the decommissioning process. It is possible that certain
conditions disallow the facility from actually leaving the simulation,
e.g. there may be left over fuel at a reactor that must leave the
facility. As with the building phase, the decommisioning phase must
involve decisions that independent of any other decommissioning
decisions, i.e. they cannot depend on one another.

Execution Phase
---------------

This phase mirrors the execution phase in the pre-resource exchange
step. It requires the same guarantee, i.e., any actions that occur
during the execution phase must be independent of all other such
actions.

.. [Strotz] Strotz, R. H. “Cardinal Utility.” The American Economic Review (1953): 384–397.

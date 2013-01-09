
Simulation Environment Overview
===============================

Timer 
------

The simulation environment for defining and solving the matching problem between 
supply and demand occurs in Phases within the Cyclus environment.

Phase 1 : Requests 
******************

Information passing phase. Consumers issue requests. All are routed to all suppliers. 
These requests for bids must conform with the attributes and limitations imposed by Institutions and Regions. 

Phase 2 : Bids
**************

For each request, given the specifications of the request, each supplier issues 
a bid to each consumer (note, the sum of all bids may be larger than the 
capacity of the supplier, though ideally no single bid will be larger than the 
capacity of the supplier)

These bids must conform with the attributes and limitations imposed by Institutions and Regions. 

Not all requests have to be responded to, so not all RFBs will receive bids. 
Those RFBs not receiving bids will not be matched on an arc. 

Phase 3 : Preference Assesment
******************************

Now, each consumer assesses the set of bids received and through some magic 
determines a vector :math:`\alpha_{ij}` of affinities for each supplier.

These preferences must conform with the attributes and limitations imposed by Institutions and Regions. 

Phase 4 : Matching
*********************

Then, we solve the problem based on the collected :math:`\alpha_{ij}` vectors, 
the matches are send.  

Possibly, some of these will **not** require network flow solutions. Perhaps, 
that is, the solutions are trivial. Those matching problems, if solvable simply, 
should be solved simply via some heuristic. For this reason, some simple 
simulations will never require network flow solves.

Generally, the matching problem is described by a linear program that attempts 
to match all the supplier offers with consumer requests.  Here is a simple 
example in which materials are being exchanged.

.. math::

  S_i i\in[1,I] = \verb|supplier offer |i

  {S_i(m,q)} = \verb|set of supplier offers, a function of mass and quality|

  D_j j\in[1,J] = \verb|consumer demand |j

  {D_j(m,q)} = \verb|set of consumer demands, a function of mass and quality.|


The objective function 

.. math::
  
  min \sum_i \sum_j \alpha_{ij}(i,j,q_i,q_j)x_{ij}
  
where

.. math::

  \alpha_{ij} = cost

is subject to the constraints based on offer and request amounts 

.. math:: 
  
  s.t. \sum_j x_{ij} - \sum_i x_{ji} = b_{ij}

  s.t. \sum_j x_{ij} = D_i

  s.t. \sum_i x_{ji} = S_j


Arbitrary constraints might be modeled with preference matrices :math:`\beta_j` 
etc.

.. math::

  s.t. \sum_j \beta_{ij} x_{ij} = \sum_j \gamma_{ij} x_{ij} + \sum_i\sum_j \delta_{ij} x_{ij}   


Since in a multi-regional, multi-facility simulation, the preferences 
:math:`\alpha_{ij}` may be a function of a great number of parameters, this 
vector can only be determined by some combination of the facility, institutions, 
and regions making the request.

In order to do this, we need to make it possible for all agents, i, to provide 
an :math:`\alpha_{j}` to the market. This is the driving reason that the agent 
must therefore have information about the list of j offer/requests.

Phase 5 : Send Orders
*********************

The resource object that is described in the matched order is then sent from the 
supplying facility to the receiving facility. 



Messages
--------

To facilitate these phases, messages must carry information between agents for 
each step.

Requests for Bids (RFB)
***********************

Consumers should issue just one RFB per consumer, per item that they want.  

It contains sufficient information about the Resource desired to assist the 
supplier in formulating a bid.


Bids
*****

If a supplier is interested in satisfying the request, it creates a Bid that 
contains sufficient information about the Resource to be sent to assist the 
requester in the process of prefering this over over others. 

The bid also contains a price. (Perhaps we need a price class that has some 
richness, fixed cost, variable cost... TBD).

The bid may also send a bid constraint overall to the matchmaking entity in 
order to avoid overpromising. That is, if a facility receives many requests, it 
may desire to return bids which sum to an amount above its total capacity. It 
must indicate to the matchmaking facility what its capacity constraint actually 
is so that it is not matched to so many requests that it overwhelms its 
capacity.

Preferences 
***********

A vector of Preferences, :math:`\alpha_{ij}`, is generated, based on the Bids. 
This is a vector of doubles normalized so that no single facility unfairly 
dominates in a market. 

Within these preference coefficients, one arc may be unallowable. This will be 
indicated by the flagged value of that coefficient.

In a package sent to the matchmacker, information about the :math:`\alpha_{ij}` 
vector, the resource specification matching the Bid resource specification, and 
function pointers to the consumer and supplier that may provide constraint 
functions. 

Order
******

When matched, the order will match the bid in quality (though perhaps only a 
fraction of the quality.) The orders are then sent down to the matched 
facilities and the sending facility meets the order according to the bid (or, 
perhaps, throws an exception if it's overestimated its capacity or something).




Agents 
-------



Resources 
---------

Resources are exchanged between agents along flow paths defined by in and out 
commodities. Supply and demand of those commodities is managed by the matching 
system generally described by the linear prgram (LP). 


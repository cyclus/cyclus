
Simulation Environment Overview
===============================

Timer 
------

The timer remains.

Agents 
-------



Resources 
---------

Resources are exchanged between agents along flow paths defined by in and out 
commodities. Supply and demand of those commodities is managed by a matching 
system generally described by a linear prgram (LP). 

Supply-Demand Linear Program
*****************************

The linear program attempts to match all the supplier offers with consumer requests.
Here is a simple example in which materials are being exchanged.

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
:math:`\alpha_{ij}` may be a function of a great number of parameters. This

The responsibility is up to the user to determine how an agent creates its 
preference vector, :math:`\alpha_{ij}`.


In order to do this, we need to make it possible for all agents, i, to provide 
an :math:`\alpha_{j}` to the market. The agent must therefore have information 
about the list of j offer/requests.

Transaction Flow
----------------

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

Phase 3 : Preference Assesment
******************************

Now, each consumer assesses the set of bids received and through some magic 
determines a vector :math:`\alpha_{ij}` of affinities for each supplier.

These preferences must conform with the attributes and limitations imposed by Institutions and Regions. 

Phase 4 : Matching
*********************

Then, we solve the problem based on the collected :math:`\alpha_{ij}` vectors. 

Possibly, some of these will **not** require network flow solutions. Perhaps, 
that is, the solutions are trivial. Those matching problems, if solvable simply, 
should be solved simply via some heuristic. For this reason, some simple 
simulations will never require network flow solves.


Messages
--------

To facilitate these phases, messages must carry information between agents for 
each step.

Requests for Bids (RFB)
***********************

Consumers should issue just one RFB per consumer, per item that they want.  


Bids
*****


Order
*****


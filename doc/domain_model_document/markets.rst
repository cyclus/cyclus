

The linear program attempts to match all the supplier offers with consumer requests.

.. math::

  S_i i\in[1,I] = \verb|supplier offer |i

  {S_i(m,q)} = \verb|set of supplier offers, a function of mass and quality|

  C_j j\in[1,J] = \verb|consumer request |j

  {C_j(m,q)} = \verb|set of consumer requests, a function of mass and quality|


The objective function 

.. math::
  
  min \sum_i \sum_j \alpha_{ij}(i,j,q_i,q_j)x_{ij}
  
where

.. math::

  \alpha_{ij} = cost

is subject to the constraints based on offer and request amounts 

.. math:: 
  
  s.t. \sum_j x_{ij} - \sum_i x_{ji} = b_{ij}

  s.t. \sum_j x_{ij} = m_i

  s.t. \sum_i x_{ji} = m_j


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

Phase 1 : Requests 
------------------

Information passing phase.  Consumers issue requests. All are routed to all suppliers.

Phase 2 : Bids
--------------

For each request, given the specifications of the request, each supplier issues 
a bid to each consumer (note, the sum of all bids may be larger than the 
capacity of the supplier, though ideally no single bid will be larger than the 
capacity of the supplier)


Phase 3 : Preference Assesment
------------------------------

Now, each consumer assesses the set of bids received and through some magic 
determines a vector :math:`\alpha_{ij}` of affinities for each supplier.

Phase 4 : Matching
---------------------

Then, we solve the problem based on the collected :math:`\alpha_{ij}` vectors. 

Possibly, some of these will **not** require network flow solutions. Perhaps, 
that is, the solutions are trivial. Those matching problems, if solvable simply, 
should be solved simply via some heuristic. For this reason, some simple 
simulations will never require network flow solves.

Simulation Environment Overview
===============================

Timer 


Agents 


Resources 

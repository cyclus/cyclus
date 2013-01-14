
Timer 
------

The simulation environment for defining and solving the matching problem between 
supply and demand occurs in Phases within the Cyclus environment.

 * commodity markets exist with consumer agents and supplier agents,
 * consumer agents issue requests for resources to all suppliers,
 * supplier agents issue bids to offer resources in response to the
   requests, including information about price,
 * consumer agents evaluate the responding bids according to their own
   assessment of costs,
 * commodity markets collect the consumer agent assessments of the
   relative peference of each bid and match to optimize matching
   consumers with suppliers according to their preference, and
 * commodity markets issue purchase orders to suppliers.


Phase 1 : Requests 
******************

Information passing phase. Consumers issue requests. All are routed to all
suppliers.  These requests for bids must conform with the attributes and
limitations imposed by Institutions and Regions. 

Phase 2 : Bids
**************

For each request, given the specifications of the request, each supplier issues
a bid to each consumer (note, the sum of all bids may be larger than the
capacity of the supplier, though ideally no single bid will be larger than the
capacity of the supplier)

Phase 3 : Preference Assesment
******************************

Now, each consumer assesses the set of bids received and through some magic 
determines a vector :math:`\alpha_{ij}` of preferences for each supplier.

Phase 4 : Matching
*********************

Then, we solve the problem based on the collected preference
:math:`\alpha_{ij}` vectors, and the matches are sent.  

Phase 5 : Send Orders
*********************

The resource object that is described in the matched order is then sent from the 
supplying facility to the receiving facility. 


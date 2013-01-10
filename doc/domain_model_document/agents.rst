Agents
=======

Cyclus agents, in general, combine a variety of different behaviors
into single objects.  By design, each behavior should be inherited by
the agents from a different and well-contained parent class:

 * DynamicModule: A fundamental feature of Cyclus is that users can
   select alternative implementations of agents at runtime by
   implementating dynamic loading.
 * Communicator: Agents will communicate with each other using a
   common set of methods to pass :doc:`Messages`.
 * TimeHandler: Agents will respond to triggers from the :doc:`Timer`
   using a common set of methods.
 * Prototype: Some agents will be replicated many times in a
   simulation from a single prototype.
 * PolicyRelationships: Agents will have relationships to other agents
   that impose economic and political policy limitations/preferences
   on their ability to trade resources.





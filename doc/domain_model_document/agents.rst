
Agents
=======

Cyclus agents, are an abstraction for entity's that (dynamically) participate
and interact in a simulation.  Cyclus will standardly support three types of
agents: :doc:`facilities<facility>`, :doc:`institutions<institution>`, and
:doc:`regions<region>`.  Facilities operate within their parent institutions'
preferences and institutions operate within their owning regions' preferences.

Agents have the following properties:

  * Dynamically loadable: A fundamental feature of Cyclus is that users can
    share and use alternative implementations of agents in simulations with
    relative ease (i.e. without source code sharing recompiling, linking, etc)

  * Communication: Agents will communicate with each other using a canonical
    :doc:`messaging system<messages>`.

  * Time response: Agents will respond to standardized triggers from the :doc:`timer`

  * Replication: Some agents will act as prototypes being replicated many times in a
    simulation (i.e. mid-simulation agent deployment).

  * Policy Relationships: Agents will have relationships with other agents that
    may impose economic and/or politically motivated constraints/preferences
    with respect to resource exchange.



.. title:: FACILITY title card

Facility
=========

In Cyclus, facilities are simulation agents that form the simulation
foundation by participating directly in resource exchange.  Facilities have
the following properties/behavior:

  * Are simulation :doc:`agents<agents>`.

  * Participate in :doc:`resource exchange<resource_exchange>` directly by sending and receiving RFBs,
    bids, and resources.

  * Have an owning or parent agent referred to as an
    :doc:`institution<institution>`.  By convention, agents' bids, RFBs, and
    resource transfers will comply with constraints imposed by their parent
    instutions prior to sending/receiving.

  * Potentially create resources, perform resource transformations, or other
    interesting desired behavior.

Examples of possible facilities:

  * Power reactor:

    - requests and receives fresh UO2 fuel
    - transforms into spent fuel, producing electricity
    - offers and sends electricity and spent fuel

  * Enrichment facility:
      
    - requests and receives uranium and fluorine
    - transforms into enriched uranium
    - offers and sends enriched uranium


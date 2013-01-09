
Messages In Cyclus
=================================

Agents in Cyclus communicate resource requests and bids, and other
information by creating and sending messages.

* There is no guaruntee that messages will arrive at their destination.

* Messages contain:

  - Payload (RFB, bid, preferences, ...)

    + Resource specification
    + Price (bids) or cost (preferences)
    + consumer/supplier RFB/bid constraints 

  - Routing information:

    + creator/sender
    + destination/receiver



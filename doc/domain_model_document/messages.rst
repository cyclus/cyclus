
Messages In Cyclus
=================================

Agents in Cyclus communicate resource requests, bids, and other information
by creating and sending messages. Messages *will* be received by the
specified receiver. Sent messages are routed directly to the receiver.

* Messages hold:

  - Payload of one or more of the following

    + Resource specification
    + Price (bids) or cost (preferences)
    + consumer/supplier RFB/bid constraints 

  - Routing information:

    + creator/sender
    + destination/receiver




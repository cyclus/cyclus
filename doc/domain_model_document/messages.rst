
Messages In Cyclus
=================================

:doc:`Agents <agents>` in Cyclus communicate resource requests, bids, and other
information by creating and sending messages.  Messages are also used to
communicate and enforce inter-agent imposed constraints (e.g. parent region
trade policy).

Message properties:

  * Sent messages *will* be received by the specified receiver.
  * Sent messages are routed directly to the receiver.
  * Messages contain:

    - Payload of one or more of the following

      + Resource specification
      + Price (bids) or cost (preferences)
      + consumer/supplier RFB/bid constraints 
      + other pertinent information

    - Routing information:

      + creator/sender
      + destination/receiver




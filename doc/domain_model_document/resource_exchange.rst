
Resource Exchange Communication
================================

Messages convey request and bid information between agents.  Exchange
negotiation occurs between two agents referred to as the consumer and
supplier. These requests and bids are constructed from basic components:

* :term:`Resource Specification`

* Price Description: Compensation required by a supplier for sending a
  bid resource to a consumer.

* Preference Description:

Request for Bid
-----------------

In order to acquire resources, consumers must send a Request for Bid (RFB)
to each acceptable supplier. An RFB contains a desired resource
specification.

Bids
-------

Suppliers respond to RFB's with bids. Bids contain a providable resource
specification and a price description.  The providable resource
specification may differ from the RFB resource specification.  Bids are
sent by suppliers to consumers. Consumers will know the originating RFB for
each bid they receive.

Consumer Preferences
---------------------

After receiving a set of bids for a desired resource, consumers will
specify a relative preference for each bid/supplier. This preference set
will be used with other competing consumer preference sets to determine the
final resource exchange obligations. The preferences are sent to the
corresponding commodity exchange for final matching.


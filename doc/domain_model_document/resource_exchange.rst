
Overview
========

The purpose of the resource exchange process is to model the
consumer-supplier interaction in a supply chain, more specifically the
nuclear fuel cycle. The process is complicated by the dynamic nature
of the simulation, i.e. actors can enter or leave the exchange at any
given time step. The most complicating factor, however, is due to the
nature of the nuclear engineering domain: the main resource being
exchanged is material which is made up of various isotopes. These
constraints lead us to a request-for-proposal/bid/bid-selection
procedure for determining the resource transactions at any given
timestep.

Motivation
==========

Because we care about individual isotopes, a full treatment of the
problem requires we take into account the full isotopic makeup of
materials during this process. Put another way, one cannot simply call
enriched uranium a product, as is normally done in these types of
matching algorithms (where, for example, a producer knows it needs a
certain type of widget). Rather, we must both specify the type of
product (e.g. enriched uranium) along with a specification (e.g.  4
w/o U-235). After an extensive literature review, the development team
has found only two other sources that must tackle a similarly
difficult problem. In [Holmgren]_, the additional source of difficulty
lies in the need to specify both a product requirement and
transportation link. They provide a framework that requires proposals
for a product specification (including time constraints), and various
suppliers provide bids on that proposal. A similar approach is used by
[Julka]_ in the chemical process industry. We provide an analgous
framework where the isotopic specification is provided in the
proposal. The dynamic nature of the simulation in conjunction with the
complex interactions due to isotopic considerations disallows
relatively simple multi-period solutions as proposed by [Quelhas]_.

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

In order to acquire resources, consumers must send a Request for Bid
(RFB) to each acceptable supplier. An RFB contains a desired resource
specification.

Bids
-------

Suppliers respond to RFB's with bids. Bids contain a providable
resource specification and a price description.  The providable
resource specification may differ from the RFB resource specification.
Bids are sent by suppliers to consumers. Consumers will know the
originating RFB for each bid they receive.

Consumer Preferences
---------------------

After receiving a set of bids for a desired resource, consumers will
specify a relative preference for each bid/supplier. This preference
set will be used with other competing consumer preference sets to
determine the final resource exchange obligations. The preferences are
sent to the corresponding commodity exchange for final matching.

.. [Holmgren] Holmgren, J., P. Davidsson, J. A. Persson, and L. Ramstedt. “An Agent Based Simulator for Production and Transportation of Products.” In The 11th World Conference on Transport Research, Berkeley, USA, 8–12, 2007.
.. [Julka] Julka, N., R. Srinivasan, and I. Karimi. “Agent-based Supply Chain Management-1: Framework.” Computers & Chemical Engineering 26, no. 12 (2002): 1755–1769.
.. [Quelhas] Quelhas, A., E. Gil, J. D. McCalley, and S. M. Ryan. “A Multiperiod Generalized Network Flow Model of the US Integrated Energy System: Part I—Model Description.” Power Systems, IEEE Transactions On 22, no. 2 (2007): 829–836.



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
time-step.

Motivation
==========

Because we care about individual isotopes, a full treatment of the
problem requires we take into account the full isotopic makeup of
materials during this process. Put another way, one cannot simply call
enriched uranium a product, as is normally done in these types of
matching algorithms (where, for example, a producer knows it needs a
certain type of widget). Rather, we must both specify the type of
product (e.g. enriched uranium) along with a specification (e.g.  4
w/o U-235).

After an extensive literature review, the development team has found
only two other sources that tackle a similarly difficult problem. In
[Holmgren]_, the additional source of difficulty lies in the need to
specify both a product requirement and transportation link. They
provide a framework that requires proposals for a product
specification (including time constraints), and various suppliers
provide bids on that proposal. A similar approach is used by [Julka]_
in the chemical process industry in which bids for crude oil are
ranked by a user-provided benchmark. We provide an analogous framework
where the isotopic specification is provided in the proposal. The
dynamic nature of the simulation in conjunction with the complex
interactions due to isotopic considerations disallows relatively
simple multi-period solutions as proposed by [Quelhas]_.

Resource Exchange Structure
===========================

The resource exchange is comprised of:

* a set of consumer resource specifications
* for each consumer resource specification, a set of response bids
  provided by producers
* for each specification/bid pair, an institution preference
* for each specification/bid pair, regional preferences and
  constraints

The amalgamation of these specifications, bids, constraints, and
preferences result in a transportation network that is solved via
linear programming (LP) and mixed integer-linear programming (MILP)
techniques. The solution of such a program results in orders,
according to which producers send resources to consumers.

This structure, including input and output, are formulated via
information that is passed around entities in the simulation using
Messages.

Resource Exchange Communication
===============================

Messages convey request and bid information between agents.  Exchange
negotiation occurs between two agents referred to as the consumer and
supplier, and can be informed by both agent's institutions and
regions.  The types communication described in the following sections
are sent by agents at any time step in which there is demand for a
resource by a consumer agent. 

Request for Bid
---------------

In order to acquire resources, consumers must send a Request for Bid
(RFB) to each desired potential supplier. An RFB contains a desired
:term:`Resource Specification`. RFB's may be ignored by suppliers.

Bids
----

Suppliers may respond to RFB's with bids. Those RFBs not receiving
bids will not be matched on an arc.  Bids contain a providable
:term:`resource Specification` and a :term:`Price Description`.  The
providable resource specification may differ from the RFB resource
specification.  Bids are sent by suppliers to consumers. Consumers
will know the originating RFB for each bid they receive.

Preferences
-----------

After receiving a set of bids for a desired resource, consumers will
generate a :term:`Preference Description` that will be used with other
preference descriptions by a market's consumers to determine the final
resource exchange obligations. 

This preference description will contain a consumer's relative
preference for each bid/supplier. Within these preference
coefficients, there may be bids a consumer wishes to reject completely
(unallowable arcs). 

In addition, the preferences will contain other constraints imposed by
the supplier and/or consumer.  Notably, suppliers will need to
communicate capacity constraints to the market.  This is a potentially
non-trivial description of resource exchange capacity for the current
time-step's bids.  This must be able to handle scenarios where the
capacity is a function of bid-varying properties.  For example, an
enrichment facility may have a capacity of SWU that is equivalent to
varying amounts of resource mass depending on both a tails enrichment
and a supplier's desired product enrichment.

Order
-----

After specifications and bids have been resolved, orders will be
generated and communicated to suppliers. This process is described in
the timer's :ref:`orders_phase`.

.. [Holmgren] Holmgren, J., P. Davidsson, J. A. Persson, and L. Ramstedt. “An Agent Based Simulator for Production and Transportation of Products.” In The 11th World Conference on Transport Research, Berkeley, USA, 8–12, 2007.
.. [Julka] Julka, N., R. Srinivasan, and I. Karimi. “Agent-based Supply Chain Management-1: Framework.” Computers & Chemical Engineering 26, no. 12 (2002): 1755–1769.
.. [Quelhas] Quelhas, A., E. Gil, J. D. McCalley, and S. M. Ryan. “A Multiperiod Generalized Network Flow Agent of the US Integrated Energy System: Part I—Agent Description.” Power Systems, IEEE Transactions On 22, no. 2 (2007): 829–836.


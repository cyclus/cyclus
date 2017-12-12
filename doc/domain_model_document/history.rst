Motivation & History
----------------------

The motivation of Cyclus grew from a desire to extend the kinds of
fuel cycle scenario simulations beyond the capabilities of the
previously existing tools.  The following specific user interactions
were considered too difficult to pursue by extending those tools:

  * creative arrangement of nuclear facilities in innovative fuel
    cycles by "mixing and matching" from a portfolio of options, 
  * tracking of discrete material objects for the purpose of following
    the pedigree of materials as they moved through the cycle, and
  * agenting individual facilities in a discrete fashion, related in
    part to the needs of agenting discrete material objects, but also
    due to some use cases such as disruption studies.

There are other requirements that were important in Cyclus'
development history that may have been possible within the frameworks
of previous tools:

  * Multiple poltical/geographic regions to support an understanding
    of the international nature of the nuclear fuel cycle
  * Open access to draw together a collaborative community of developers
  * Ease of use for a range of user sophistications in a single platform

Creative Arrangement of Innovative Fuel Cycles
==============================================

Many of the predecessors to Cyclus adopted a systems dynamics approach,
building fuel cycle agents in drag-and-drop environments using
primitives of stocks and flows.  These tools were adopted due to the
natural analog to the material flow problems fundamental to fuel cycle
agenting, and the ease of creating such flow paths.  In practice, as
more complexity became necessary in these simulations, the use of
simple stocks and flows to represent facilities and the material
flows between them was no longer possible.  Many flows, stocks and
related parameters were needed to agent individual facilities and some
advanced methods had to be programmed into certain of those primitives
to accomplish the requirements of the desired analysis.  The interface
between the collections of primitives that represented each component
of the fuel cycle emerged by circumstance rather than by design, and
the entire paradigm was constrained to the time step ODE solution
paradigm of those system dynamics tools.

If one desired to replace the agent of the reactor (actually a fleet
of reactors) with an alternative agent exhibiting different behavior,
it was necessary to formulate that agent in the context of the systems
dynamics software and either ensure that the agent conformed to the de
facto interface with the remainder of the system, or modify the entire
system to change that interface.  If the interface did change, it
became more difficult to compare two simulations using different
reactor agents because the underlying simulation was somehow
perturbed.

From this emerged the software requirement to design strict interfaces
for the individual components of a simulation and allow developers to
implement the behavior of their choice for those components as long as
they conformed to the interface definition.  With this strategy, it
would be possible to exchnage individual modules that each represented
the same component but with different behavior and perform direct
comparisons of the impact of that single perturbation.  These notions
of encapsulation and polymorphism are fundemantal to object-oriented
programming practices, leading to an object-based agent for the
development of Cyclus.

Discrete Materials Exchanged Among Discrete Facilities
======================================================

In general, the systems analysis tools that preceded Cyclus treated
the flow of materials as continuous flows of isotopic mixtures among
fleets of identically performing facilities.  Material that entered a
fleet of facilities would be disconnected from material leaving that
same fleet other than by the causal connection implied by the
progression through time.  There was no way to trace that a particular
quanta of material had proceeded through a flow path that ivolved a
specific set of facilities over a specific history. Some uses cases
found this to be an interesting type of analysis, particularly with
questions of material attribution in potential theft and diversion use
cases.  Doing so would necessitate quantized material objects with
unique identifying characteristics.  Furtheremore, there was little
value in representing discrete quanta of material without being able
to respresent the history of discrete facilities through which that
material had passed.

Discrete facility agenting has additional use cases beyond simply
supporting the agenting of discrete materials.  In particular, where
disruptions in the operation of a single facility can have an
important impact on the overal system performance, it is necessary for
a agenting paradigm that supports those disruption.  

From this emerged the software requirement to agent the behavior of
individual facilities exchanging discrete quanta of material
throughout time.

From a theoretical point of view, one can imagine the object-based
Cyclus components interacting as mathematical operators in a large set
of equations, the desire to agent discrete materials and facilities
leads instead to components that interact by exchange objects, whether
information objects of material objects. [#mc_analog]_ Whereas sets of
equations lend themselves more naturally to agenting a continuum, this
kind of information passing object paradigm is more natural for the
discrete material tracking through discrete facilties.

Region Ownership/Priority
==========================
  * Multiple poltical/geographic regions to support an understanding
    of the international nature of the nuclear fuel cycle

Open Access Software and Tools
==============================

Given the limited resources (financial and/or human) for development
of nuclear fuel cycle systems analysis tools, it is valuable to
facilitate a collaborative development agent in which those resources
are put towards the development of a compatible set of technologies.
One of the strongest ways to support his is with open access software
developed upon open access tools.  

Supporting a Range of User Sophistication
===========================================

One of the challenges in any software, but particularly for
policy-drive systems analysis, is to strike a balance between the
agenting fidelity and accuracy desired by advanced users and the ease
of use necessary for less sophisticated users. 

Selection of C++
==================
 * performance & broad experience
 * barrier to adoption


.. [#mc_analog] To some degree this choice can be regarded as analoguous to
       the choice between deterministic and stochastic radiation
       transport methods.


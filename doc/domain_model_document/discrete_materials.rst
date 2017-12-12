Discrete Materials in Cyclus
============================

One of the defining features of Cyclus among other fuel cycle
simulators is the treatment of materials as discrete quantized objects
that can be traded and tracked throughout the simulation. 
The many :doc:`use cases<use_cases>` in which such a feature is necessary
provide a basis for this implementation of a discrete and
quantized nature of materials. 

Always Countable, Sometimes Divisible
=====================================

Implicit to the notion of discrete and quantized materials are the
concepts that those materials are both indivisible and countable.
Since there some cases where it is logical to transact materials in
quantities that are divisible and/or measured in cotninuous units,
some discussion is necessary determine how those can co-exist in the
same simulation.

A primary motivation for discrete and quantized materials is the
ability to study the flow of material and the attribution of those
materials through prior ownership.  Such a notion is clear when
refering to real world objects that are clearly defined in a discrete
and quantized way.  Perhaps the most obvious example in a fuel cycle
scenario is that of a nuclear fuel assembly.  A nuclear fuel assembly
for a given reactor type has a clear definition of its mass and can be
treated, for the most part, as indivisible and countable.

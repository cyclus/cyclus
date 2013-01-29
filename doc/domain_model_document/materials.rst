
.. summary Some developers notes on how materials and isotopic vectors work

.. _materials_and_istotopes:

Materials and Isotopes in Cyclus
================================

Cyclus tracks many objects and :doc:`Resources <resources>` , but materials 
remain the primary unit of information in the simulator. The movement, 
transmutation, and capital exchange of materials can be used to inform many 
metrics of interest to fuel cycle analysis.  Mass flows can be translated in 
post-processing into economic, environmental, policy, and health related fuel 
cycle metrics.  Facility inventories, radio toxicity, decay heat, waste volumes, 
PRPP indices, etc. can all be found from mass flow data.  Many fuel cycle 
simulation tools only track fuel materials for this reason. 

Generic Resource Characteristics
---------------------------------

Materials are a well defined type of conserved :doc:`Resource <resources>` in 
the Cyclus framework.  Thus, all materials posess a :ref:`generic  Resource 
quantity <resource_quantity>` . That is, an amount and a default unit. For 
Materials, the default unit is kilograms and the total mass of the material is 
therefore the generic Resource amount of any material.

Material Resource Characteristics
---------------------------------

The main :ref:`quality characteristic <resource_quality>` tracked by Material 
Resources is their isotopic composition and history. This is acheived with a 
combination of the **IsoVector** and **CompMap** concepts. These concepts are 
the engine with which Material objects keep track (simultaneously) of atomic 
[mol] and mass [kg] -based isotopic vectors. 

Materials are the primary Resource that is transacted in a *Cyclus* simulation. 
The primary Material-specific :ref:`resource quality characteristic <resource_quality>` is isotopic composition.
Isotopic compositions can be expensive to copy and to check equality, and are therefore
decomposed into two containers. CompMaps are the most basic isotopic container, and
can be thought of as a simple map of isotopic integer values (the keys) to values.
An IsoVector manages a smart pointer to a CompMap in order to minimize unneccessary
operations.


CompMaps
--------

CompMaps are immutable objects: once constructed, they are guaranteed not to change. This
design decision provides confidence to developers and thread safety.

*Cyclus* provides public API for CompMaps only through the use of boost smart pointers, 
typedefed as "CompMapPtr"s.

CompMaps are constructed as either an atom-based or mass-based. An example of such a construction
is provided: ::

   CompMapPtr c_p = CompMapPtr(new CompMap(MASS));
   (*c_p)[92235] = 10.0; // 10 g_235/ 100 g_comp
   (*c_p)[92238] = 90.0; // 90 g_238/ 100 g_comp
   c_p.normalize();
 

IsoVectors
----------

The IsoVector class is effectively a wrapper and manager of CompMapPtrs. Addition and 
subtraction operators are defined here, using specific cases of the mix() and separate()
functions.


Materials
---------

Materials are the main objects of information in *Cyclus* simulations. A Material has a quantity
and an IsoVector, comprising its composition. Materials are also passed around *Cyclus* functions
in boost smart pointers called "mat_rsrc_ptr"s. 

A Material is effectively fully initialized through construction via CompMapPtr or IsoVector and
setting its quantity. For example: ::

    mat_rsrc_ptr m_p = mat_rsrc_ptr(new Material(c_p));
    m_p->setQuantity(10); // 10kg of c_p


Material Data 
*************

The data held by a material object is straightforward. The interface allows 
access to the mass or atomic composition of the material ::

    m_p->mass();   // returns the total mass of the material in kg
    m_p->atoms();  // returns the total atoms in the material in moles

While the default is to sum over all contained isotopes, the data can be 
retrieved for a single isotope, ::

    m_p->mass(92235, KG);   // returns the mass of 235U in kg
    m_p->atoms(92235);      // returns the mass of 235U in moles

It is capable of returning masses in kg or g, and capable of returning atoms in moles. ::

    m_p->mass(KG);  // returns the total mass of the material in kg
    m_p->mass(G);   // returns the total mass of the material in g
    m_p->atoms();   // returns the total atoms in the material in moles


Any time the data is queried, the Material object should be decayed, so that the 
most up-to-date material is returned. 


Material Methods  
****************

The methods that can be performed on a Material object in order to mainupulated 
are a small subset of mass conserving functions. These functions allow the user 
to combine two materials, extract a material from another, and decay materials 
on demand. 

**Material absorption** can be used when two materials should be combined physically
and when their histories should be shared in the future. To use this, the material 
to absorb must already exist. This helps with mass conservation ::

   mat_rsrc_ptr other_mat = mat_rsrc_ptr(new Material(c_p));
   m_p->absorb(other_mat);

**Material extraction** can be used when a subpart of a material is being separated 
physically from the original and when that subpart will have its own history (rather 
than a shared history) in the rest of the simulation. 


There are three ways to extract a material. The first method should be used when the 
material to extract already exists. This helps with mass conservation as the function
throws an error if the old material does not contain sufficient masses of each isotope 
in the new_mat ::

   mat_rsrc_ptr other_mat = mat_rsrc_ptr(new Material(c_p));
   m_p->extract(other_mat); 

The second way extracts a specified mass of material from the original. It internally
creates a new material that has the same normalized composition as the original but has 
the specified mass. Then, this function internally calls extract(new_mat) and returns 
the new material. ::

   double mass = 1000;
   MassUnit units = KG;
   mat_rsrc_ptr new_mat = extract(mass, units); 

An analgous function exists for extracting based on number of atoms. ::

   extract(atoms, units) 


 
Material Expectations 
*********************

The material class makes two primary promises. 

First, it will always decay material sufficiently that any retrieved data is 
current.  Second, it will always conserves mass, never creating material out 
of nothing.



RecipeLogger
------------

Predefined recipes are managed by the RecipeLogger in *Cyclus*. A CompMapPtr to a specific
recipe can be acquired through its static Recipe() method.


----

**What follows is to be merged with the above.** 

----

Simulation Handling of Materials
--------------------------------

Material Plurality
******************

Cyclus is capable of recording and tracking the composition histories
of hundreds of thousands of material objects.
                                        
Basis
.....

In order to accurately model tens of thousands of fuel cycle facilities
and their associated material flows, the simulation must be capable of modeling
at least a factor of fifty more material objects than facilities. That is, if a
material object is the size of a batch of fuel, a reactor with an 18 month
cycle will pass over fifty fuel batches in an 80 year lifetime.
                                        
Isotopic Tracking
*****************

The Cyclus framework maintains mass balances of individual isotopes and is 
capable of extending isotope tracking to include (or exclude) any specific 
isotope of interest (or disinterest).

Basis
.....

Many fuel cycle metrics rely on the isotopic content of material in the system 
and as new fuel cycle technologies progress, the effects of previously 
unimportant isotopes on fuel cycle metrics of interest often become 
non-negligible [24]. 

                                        
Mass Conservation
*****************

Mass is conserved in Cyclus Materials. The way that this is implemented 
requires the developer to utilize only conservative extract and absorb 
functions when creating and merging material.
                                        
Basis
.....

Conservation of the fundamental unit of information in a simulation
provides transparency to the developer and the user. A fundamentally sacred
unit also provides a useful tool in error checking during development. Mass
flows can be translated in post-processing into many metrics of interest to
fuel cycle analysis. Facility inventories, radiotoxicity, decay heat, waste
volumes, PRPP indices, etc. can all be found from mass flow data. [F1-F10,
F13-F16, S1-S11, E2, E3, E7-E14, M1-M8, M10, U1, U3-U5, Analysis Function
5.1.1]

Mass Conversion
***************

Cyclus is able to convert mass between mass groups due to
transmutation and decay.
                                        
Basis
.....

Lossless mass tracking requires accounting for mass changes between mass
groups. Furthermore, a fundamentally unit provides a good basis for error
checking during development. [F1-F10, F13-F16, S1-S11, E2, E3, E7-E14, M1-M8,
M10, U1, U3-U5, Analysis Function 5.1.1]
                                        
Default Material Data 
*********************
                                        
Cyclus shall provide validated preconfigured materials that describe canonical 
materials at each stage of the fuel cycle.
                                        
Most importantly, validated libraries of standard data concerning input and output fuel
compositions shall be provided.

Basis
.....

Enough data should come pre-packaged with Cyclus such that an end-user
or viewer may run a broad range of scenarios without providing any data to the
simulation. Only validated, quality controlled data should be made available
for the user to select when designing a scenario. [Analysis Function 5.1.1,
Operational Function 5.2.1 and Performance Function 5.3.3]

Providing input and output fuel recipes supports benchmarking and verification 
objectives as well as all mission objectives involving the use of standard 
reactor facility types and burnups. [F1-F4, F6-F10, F12, F13, F15-F16, S1-S11, 
E2, E3, E6-E13, M1-M6, M7, U1, U3-U5, Analysis Function 5.1.1, Operational 
Function 5.2.1 and Performance Function 5.3.3]
                                        
Decay Data
**********

Cyclus provides comprehensive and validated decay data.
                                        
Basis
.....

This supports all mission objectives involving the use of preconfigured
storage and repository models, which will rely on decay data in order to
perform appropriate transmutations of material objects at runtime. [F1, F3-F10,
F12, F14-F16, S2-S10, E2, E3, E6-E13, M2, M3, M6, M7, M14, U1, U3-U5,
Operational Functions 5.2.1 and Performance Function 5.3.3, Analysis Function
5.1.1]
                                        
                                        
Transmutation Data?
*******************

Validated libraries of standard data concerning transmutation by
irradiation must be provided.

Basis
.....

This supports all mission objectives involving the use of preconfigured
nuclear systems, which will rely on libraries of externally calculated core
physics isotopics in order to perform appropriate transmutations of material
objects at runtime. [F1-F4, F6-F10, F12, F13, F15-F16, S1-S11, E2, E3, E6- E13,
M1-M7, U1, U3-U5, Analysis Function 5.1.1, Operational Function 5.2.1 and
Performance Function 5.3.3].
                                        

Separations Data?
*****************

A library of verified and validated separations matrices, standard
reprocessing method data and process details must be provided.
                                        
Basis
.....

This supports all mission objectives involving the use of preconfigured
reprocessing facilities, which will rely on separations matrices modeling
standard aqueous (i.e. PUREX, UREX and electrochemical), pyrolitic (e.g.
electrolysis, voloxidation, or fluoride volatility), and other standard
reprocessing methods to perform appropriate transmutations of material objects
at runtime. [F1-F10, F12-F16, S1-S10, E1-E3, E8-E14, M1-M7, M10, M16, U1-U5,
Analysis Function 5.1.1, Operational Function 5.2.1 and Performance Function
5.3.3].

Chemical Forms Data?
********************

A verified library of data concerning material chemical forms, waste
forms, and material packaging must be provided.
                                        
Basis
.....

This supports all mission objectives concerned with waste
characterization which rely on preconfigured repository, storage, and
transportation models. [F1-F10, F12, F13, F15, F16, S1, S3-S10, E1-E3, E6, E7,
E9, E11, M2, M3, M6, M7, U3-U5, Analysis Function 5.1.1, Operational Function
5.2.1 and Performance Function 5.3.3] 

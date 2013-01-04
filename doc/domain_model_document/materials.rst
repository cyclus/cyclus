Materials
~~~~~~~~~

Materials are the primary unit of information in fuel cycle analysis. The 
movement, transmutation, and capital exchange of materials can be used to inform 
many metrics of interest to fuel cycle analysis.  Mass flows can be translated 
in post-processing into economic, environmental, policy, and health related fuel 
cycle metrics.  Facility inventories, radio toxicity, decay heat, waste volumes, 
PRPP indices, etc. can all be found from mass flow data.  Many fuel cycle 
simulation tools only track fuel materials for this reason. 

Cyclus tracks many objects and resources beyond matierials, but they remain the 
primary unit of information in the simulator.

Generic Resource Characteristics
---------------------------------

Materials are a well defined type of conserved Resource in the Cyclus framework.  
Thus, all materials posess the generic characteristics of Resources such as an 
amount and a default unit. For Materials, the default unit is kilograms and the 
total mass of the material is therefore the generic Resource amount of any 
material.

Material Resource Characteristics
---------------------------------

The main specific characteristic tracked by Material Resources is their isotopic 
composition and history.

Composition
***********

This feature is acheived with a combination of the **IsoVector** and **CompMap** 
concepts. These concepts are the engine with which Material objects keep track 
(simultaneously) of atomic [mol] and mass [kg] -based isotopic vectors. 

The Material keeps track of its IsoVector history throughout the simulation. 

the IsoVector is


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

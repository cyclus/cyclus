Materials
~~~~~~~~~

Materials are the primary unit of information in fuel cycle analysis. The 
movement, transmutation, and capital exchange of materials can be used to inform 
economic, environmental, policy, and health related fuel cycle metrics.  With 
simplifying assumptions, mass flows can be converted into most fuel cycle 
metrics of interest.  Many fuel cycle simulation tools only track materials for 
this reason. 

(ugh... combine these paragraphs)

Mass flows can be translated in post-processing into many metrics of
interest to fuel cycle analysis. Facility inventories, radio toxicity, decay
heat, waste volumes, PRPP indices, etc. can all be found from mass flow data.
The basic unit of information in Cyclus is therefore nuclear material
movement and transmutation.

Material Resource Characteristics
---------------------------------

Materials are a well defined type of conserved Resource in the Cyclus framework. 

Material Composition Tracking
-----------------------------

Cyclus should be capable of recording and tracking the composition histories
of hundreds of thousands of material objects.
                                        
In order to accurately model tens of thousands of fuel cycle facilities
and their associated material flows, the simulation must be capable of modeling
at least a factor of fifty more material objects than facilities. That is, if a
material object is the size of a batch of fuel, a reactor with an 18 month
cycle will pass over fifty fuel batches in an 80 year lifetime.
                                        
Isotopic Tracking
*****************

Since many fuel cycle metrics rely on the isotopic content of material in the 
system, the Cyclus framework maintains mass balances of individual isotopes.
As new fuel cycle technologies progress, the effects of previously
unimportant isotopes on fuel cycle metrics of interest often become
non-negligible [24]. 

Similary, the Cyclus must be capable of extending isotope tracking to include (or
exclude) any specific isotope of interest (or disinterest).
                                        
Mass Tracking
*************

A-7.2.1 Mass must be conserved in Cyclus.
                                        
Basis: Conservation of the fundamental unit of information in a simulation
provides transparency to the developer and the user. A fundamentally sacred
unit also provides a useful tool in error checking during development. Mass
flows can be translated in post-processing into many metrics of interest to
fuel cycle analysis. Facility inventories, radiotoxicity, decay heat, waste
volumes, PRPP indices, etc. can all be found from mass flow data. [F1-F10,
F13-F16, S1-S11, E2, E3, E7-E14, M1-M8, M10, U1, U3-U5, Analysis Function
5.1.1]

A-7.2.2 Cyclus must be able to convert mass between mass groups due to
transmutation and decay.
                                        
Basis: Lossless mass tracking requires accounting for mass changes between mass
groups. Furthermore, a fundamentally unit provides a good basis for error
checking during development. [F1-F10, F13-F16, S1-S11, E2, E3, E7-E14, M1-M8,
M10, U1, U3-U5, Analysis Function 5.1.1]
                                        
A-7.3 Material Data
                                        
A-7.3.1 Cyclus shall provide validated preconfigured modules for each stage of
the fuel cycle.
                                        
Basis: Enough data should come pre-packaged with Cyclus such that an end-user
or viewer may run a broad range of scenarios without providing any data to the
simulation. Only validated, quality controlled data should be made available
for the user to select when designing a scenario. [Analysis Function 5.1.1,
Operational Function 5.2.1 and Performance Function 5.3.3]
                                        
A-7.3.2 Cyclus must provide comprehensive and validated decay data.
                                        
Basis: This supports all mission objectives involving the use of preconfigured
storage and repository models, which will rely on decay data in order to
perform appropriate transmutations of material objects at runtime. [F1, F3-F10,
F12, F14-F16, S2-S10, E2, E3, E6-E13, M2, M3, M6, M7, M14, U1, U3-U5,
Operational Functions 5.2.1 and Performance Function 5.3.3, Analysis Function
5.1.1]
                                        
A-7.3.3 Validated libraries of standard data concerning input and output fuel
compositions must be provided.
                                        
Basis: This supports benchmarking and verification objectives as well as all
mission objectives involving the use of standard reactor facility types and
burnups. [F1-F4, F6-F10, F12, F13, F15-F16, S1-S11, E2, E3, E6-E13, M1-M6, M7,
U1, U3-U5, Analysis Function 5.1.1, Operational Function 5.2.1 and Performance
Function 5.3.3]
                                        
A-7.3.4 Validated libraries of standard data concerning transmutation by
irradiation must be provided.
                                        
Basis: This supports all mission objectives involving the use of preconfigured
nuclear systems, which will rely on libraries of externally calculated core
physics isotopics in order to perform appropriate transmutations of material
objects at runtime. [F1-F4, F6-F10, F12, F13, F15-F16, S1-S11, E2, E3, E6- E13,
M1-M7, U1, U3-U5, Analysis Function 5.1.1, Operational Function 5.2.1 and
Performance Function 5.3.3].
                                        
A-7.3.5 A library of verified and validated separations matrices, standard
reprocessing method data and process details must be provided.
                                        
Basis: This supports all mission objectives involving the use of preconfigured
reprocessing facilities, which will rely on separations matrices modeling
standard aqueous (i.e. PUREX, UREX and electrochemical), pyrolitic (e.g.
electrolysis, voloxidation, or fluoride volatility), and other standard
reprocessing methods to perform appropriate transmutations of material objects
at runtime. [F1-F10, F12-F16, S1-S10, E1-E3, E8-E14, M1-M7, M10, M16, U1-U5,
Analysis Function 5.1.1, Operational Function 5.2.1 and Performance Function
5.3.3].

A-7.3.6 A verified library of data concerning material chemical forms, waste
forms, and material packaging must be provided.
                                        
Basis: This supports all mission objectives concerned with waste
characterization which rely on preconfigured repository, storage, and
transportation models. [F1-F10, F12, F13, F15, F16, S1, S3-S10, E1-E3, E6, E7,
E9, E11, M2, M3, M6, M7, U3-U5, Analysis Function 5.1.1, Operational Function
5.2.1 and Performance Function 5.3.3] 

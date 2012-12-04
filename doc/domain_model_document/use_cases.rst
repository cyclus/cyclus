Use Cases
=========

**Note, this is still very closely copied and pasted from the FCS requirements 
document. It needs to be re-imagined to avoid self-plaigarism.**

Cyclus seeks to support the range of studies that are of interest to an 
interdisciplinary user community. In support of these studies, it will need to 
provide an interface that accomodates a variety of input parameters and output 
metrics. This section describes the types of studies that will be explicitly 
supported by the Cyclus framework.

Technology Studies
------------------

Technology studies test the system effects of deploying a specific nuclear 
technology (usually at the facility level). These studies are performed by users 
seeking to determine the promise of a certain innovative reactor technology, 
separations process, or combination of technologies. The system-level aspects of 
their introduction to fuel cycles, new mathematical models of facility process 
dynamics, and other factors that may focus research and development goals or 
inform policies.

- Cyclus will support the user who relies on pre-configured facilities and 
  deployment scenarios, merely substituting one predefined technology for 
  another (e.g. trading PWRs for BWRs) in order to make comparisons between 
  those facilities. Cyclus will provide a library of interchangeable facility 
  configurations and scenario specifications for user selection.
- Cyclus will allow end-user specification of the parametric configuration of 
  supplied facility models. These supplied models will be prepared to  constrain 
  the user to appropriate input parameter ranges.
- Cyclus will allow advanced user customization of process behaviors in supplied 
  facility models. Input error checking is not possible for this use case, so 
  the user will be expected to constrain their input to appropriate functions 
  and values.
- Cyclus allows substitution of developer-generated facility models. That is, 
  the developer will supply all specified databases and process information for 
  the new facility model, Cycamore will supply databases and process information 
  for other facilities that surround it to complete the fuel cycle, and Cyclus 
  will provide a framework on which those facilities communicated.  


Example Use Cases
*****************

Cyclus will support investigation and analysis of:

#. Metrics resulting from deployment of various transmutation technologies
#. The “technical and economic aspects of external neutron source-driven 
   transmutation systems to inform whether future investigation in this approach is 
   warranted.” [ROAD2010]
#. Uranium/thorium resource depletion as a function of facility types, 
   deployment decisions.
#. Uranium/thorium utilization of various new once through fuels. [ROAD2010]
#. Direct SNF disposal per MWh from various improved once through fuels.
#. Metrics resulting from deployment of various separations schema (UREX, PUREX, 
   etc.)
#. Disposal metrics resulting from deployment of various partitioning and 
   transmutation, reprocessing, and storage options.
#. Disposal metrics resulting from disposal of civilian and defense used nuclear 
   fuel in “a variety of geologic environments”.
#. Effects of waste form disposition and packaging on repository and safety 
   metrics.
#. Cumulative effects of process losses in partitioning and transmutation.
#. Hydrogen generation capacities as a function of facility deployment.
#. Other waste heat utilization capacities as a function of facility deployment.
#. Transition dynamics for new facility types.
#. The need for an advanced fuel research facility and system effects of new 
   fuels and cladding capable of irradiation to higher burnup.
#. Safeguards effectiveness against covert fuel cycles.b
#. Non-Radiological releases from fuel cycle facilities (e.g. SO2 from 
   Mine/Milling) [IAEA2007].


Strategy Studies
----------------

This capability allows comparison of strategic options, using creative 
combinations of many reactor and facility types. These combinations may be 
strategic options such as once through, thermal recycling, fast reactor 
recycling, strategic facility redundancy etc. In such use cases, equivalent 
facility modules must be interchangeable and markets must support material 
routing capable of upstream material trades (such as the simulation logic of 
multipass material routing).

- Cyclus will support comparison of pre-configured strategic options, with 
  preconfigured facility deployment parameters. Such preconfigured scenarios may 
  include a Business as Usual case, or base NE Roadmap cases. The viewer may run 
  various base cases in order to make comparisons between those strategic 
  deployments. The interface must support comparison of up to 5 different cases.
- Cyclus will support end-user and advanced user specification of deployment 
  schedules and facility parameters for preconfigured routing schemes, the 
  details of which are supplied by FCS. For example, the user may want to vary 
  the fast reactor capacities and year of first deployment in a fast reactor 
  recycling scenario, but without perturbing the simulation logic of material 
  routing.
- Cyclus will allow substitution of new deployment logic and routing schemes 
  generated by the developer. The developer will supply all specified databases 
  and process information for all new business logic for material trades and 
  routing.

Example Use Cases
*****************

Cyclus will enable the strategic investigation, analysis, and demonstration of:

#. Holistic impacts of proposed deployment strategies and policy options.
#. Uranium resource depletion as a function of demand growth rates.
#. Flexibility and constraints on interim storage timing. That is, Cyclus will 
   allow comparison of safeguards detection uncertainty windows with the potential 
   magnitude of material flow differences between ordinary fuel cycles and possible 
   parallel shadow fuel cycles in which material is being diverted or covertly 
   generated.
#. Flexibility and constraints on reprocessing timing.
#. Flexibility and constraints on dry storage timing.
#. Flexibility and constraints on repository emplacement or retrieval timing
#. The magnitude of alleviated proliferation risk from limited separations 
   cycles [ROAD2010].
#. System effects of thorium fuel cycles and their synergy with existing fuel 
   cycles.
#. System effects of non-fuel material re-use (e.g. irradiated zirconium) [ROAD2010].
#. Adaptation requirements for regulatory changes Waste Acceptance Criteria, 
   Classification Bases, etc.)
#. Nuclear assisted reduction of fossil fuel use. (e.g. bio-mass processing)


Economic Studies
-----------------

Cyclus will facilitate analysis of cost and economic fuel cycle dynamics. Here, 
‘cost analysis’ refers to financial information as an output metric only whereas 
‘economic analysis’ refers to the use of costs and pricing as input parameters 
which affect simulation dynamics and even drive feedback behaviors.

- Cyclus will support cost comparison of pre-configured scenarios, using 
  FCS-calculated cost metrics. Such FCS-calculated metrics may include Fuel 
  Cycle Costs, Levelized Cost of Electricity, Total Reactor Capital costs, etc. 
  The viewer will rely on FCS-calculated pricing and cash flow parameters for 
  facilities and materials, including but not limited to uranium pricing models 
  and facility capital and operating costs.
- Cyclus will support end user and advanced user specification of economic 
  facility parameters or material pricing information as well as customized 
  calculations of cost metrics. For example, the user may want to vary the 
  capital costs of facilities to observe the effect on the levelized cost of 
  electricity. In a ‘cost analysis’ this change in facility price will not 
  affect facility deployment or material routing in any way. Due to the lack of 
  feedback, much cost analysis can be conducted as a post-processing function 
  external to the code functionality.
- Cyclus will allow the developer to extend the model to incorporate new 
  economic data (e.g. regional taxes or interest rates and capital cost 
  depreciation schedules) in order to make cost calculations.
- Cyclus will support economic analysis by being extensible enough to allow 
  appropriately customizable system dynamics in the material routing and market 
  structures. The developer will supply new data models and all new business 
  logic for material trades, while Cyclus will supply an architecture that can 
  flexibly support new material routing and facility deployment logic.

Example Use Cases
******************

Cyclus will enable the investigation and analysis of:

#. Sensitivity of total fuel cycle cost to individual facility costs
#. Sensitivity of total fuel cycle cost to reprocessing system costs
#. Sensitivity of total fuel cycle cost to construction and fuel material costs
#. Effects of various deployment parameters on levelized cost of electricity.
#. Investigate factors (transmutation technology, facility deployment, waste 
   disposition, etc.) affecting transportation, reprocessing, disposal, storage 
   costs
#. Economic impact of technologies to extend current fleet lifetimes [ROAD2010]. 
#. Investigate factors (uranium availability, transmutation technology, deployment 
   schedule, etc.) affecting power production costs.
#. Feedback effects and institutional capital limitations of facility costs on 
   facility deployment.
#. Feedback dynamics of process costs on facility deployment.
#. Feedback dynamics of disposal costs on technology deployment.
#. Feedback dynamics of disposal costs on reprocessing timing.
#. Feedback dynamics of material pricing on facility deployment.
#. Feedback effects of material prices on material routing.
#. Feedback effects of trade relationships and material availability on regional 
   technology deployment.

Multi-Regional Studies
-----------------------

Cyclus will facilitate analysis of the nuclear fuel cycle on the global, 
national, and local scale, resolving real and fictional countries, institutions 
and their interactions.

- A viewer or end-user may be interested in observing regional variation in fuel 
  cycle metrics for parametric perturbations on the Business as Usual case. 
  Relying primarily on preconfigured scenario parameters, the user may be able 
  to change a single parameter and observe the effect on their region- specific 
  metric of interest (bilateral trade frequency, material availability, fuel 
  cycle robustness, etc.). Cyclus will provide a true-to-history base-case 
  representing the history of the nuclear fuel cycle in the world, specific to 
  the world’s countries.
- An end-user may be interested in examining the effect of deployment parameters 
  such as electricity demand or regional technology availability on fuel cycle 
  metrics of interest. The user will be capable of defining the deployment 
  parameters for custom regions (fictional or otherwise), as well as choosing 
  the facility types and facility parameters available for deployment within 
  those regions. The regional simulation response variation can then be observed 
  as a function of those deployment constraints (designed to represent policy 
  decisions, international relations scenarios, synergistic fuel cycle options, 
  etc.).
- A developer conducting a multi-regional analysis may be interested in 
  extending the model to incorporate new interregional trade rules to simulate 
  their own political or economic models or incorporate new region specific data 
  (such as location).

Example Use Cases
******************

Cyclus will enable the multi-regional investigation and analysis of:

#. Fuel assuredness/reliability/security per region.
#. Movement of sensitive nuclear materials between regions.
#. Generation of sensitive nuclear materials in each region.
#. Investigate implications of international/multi-regional fuel bank scenarios.
#. Synergy and dynamics of fuel loaning and take-back schemes.
#. Implications of multi-national waste management.
#. Trade/technology symbiosis between regions with various indigenous 
   technologies.
#. Deployment responses required by regionally differentiated demand growth.
#. Employ market and political models (e.g. trade data or the Affinity of 
   Nations Index) to predict/direct interregional material and technology 
   transfers.
#. Determine proliferation risk factors (e.g., capabilities and motivations) 
   [ROAD2010].
#. Technology and materials proliferation risks as informed by social science 
   research in international security [ROAD2010].
#. Feedback dynamics of international trade relationships on fuel cycle costs, 
   regional energy security, facility deployment, etc.
#. Regional grid load restrictions.
#. Regional political deployment restrictions (e.g. state-level moratoria 
   awaiting a federal disposal plan).
#. Regional electricity pricing differences (e.g. grid-level pricing 
   dependencies).
#. Deployment effects of institutional (e.g. governments, corporations) 
   variations in available capital, technology capability, etc.
￼
Disruption Studies
-----------------------------

A user may want to study the ripple effect (or lack thereof) experienced by a 
fuel cycle in the event of an unplanned facility shutdown or process disruption.

- A viewer or end-user may be interested in observing fuel cycle responses to 
  disruption for preconfigured facilities and deployment scenarios. Relying 
  primarily on preconfigured scenarios, the user may be able to investigate the 
  relative disruption thresholds of scenarios by observing the effect of upset 
  probability on fuel cycle robustness. Cyclus will provide preconfigured base 
  case fuel cycles of interest for this comparison.
- An end-user may be interested in examining the effect of upset in custom 
  deployment scenarios. The user will be capable of defining custom deployments 
  and facility reliability probabilities. The simulation response to disruptions 
  can then be observed in relation to those deployment scenarios.
- A developer may be interested in altering the disruption responses of 
  facilities and material routing logic in order to investigate strategies (e.g. 
  redundancy alternatives, storage and staging, etc.) for improved fuel cycle 
  robustness. Cyclus will provide architecture capable of flexibly allowing 
  modifications to facility disruption responses, deployment logic, and material 
  routing schemes.

Example Use Cases
******************

Cyclus will enable the disruption analysis of:

#. Weak links in process capacity and timing during transition to new 
   technologies.
#. Fuel cycle robustness and power generation for various demand scenarios.
#. Effects of facility/process reliability on fuel cycle cost, power production, 
   etc. That is, what are the ramifications of the shutdown of a facility or 
   facilities?
#. Comparative benefits and drawbacks for storage and staging strategies and 
   redundant deployment scenarios designed to promote robustness.
#. Reliability implications of aging and degradation of system structures and 
   components, (reactor core internals, pressure vessels, building materials, 
   pipes, cables etc.)
#. Sudden changes in resource availability or price.
#. Sudden shutdown of centralized reprocessing facilities.


[ROAD2010] United States Department of Energy. Nuclear Energy Research and Development 
Roadmap. DOE-NE, http://www.ne.doe.gov/pdfFiles/NuclearEnergy_Roadmap_Final.pdf. 
April 2010.

[IAEA2007] International Atomic Energy Agency (IAEA) Guidance for the Application of 
an Assessment Methodology for Innovative Nuclear Energy Systems INPRO 
Manual-Environment, IAEA- TECDOC-1575 /Vol. 7. (2007). 1–169.

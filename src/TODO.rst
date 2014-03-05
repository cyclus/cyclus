
* [done] need to properly persist and initialize:
  
  - Model::next_id_
  - Context::trans_id_
  - Resource::nextid_
  - Composition::next_id_

* define a way to represent initial inventories for any/all agents in the
  master schema and translate them to the output db during file loading step.
  Initial agents can have initial inventories - prototypes cannot. Otherwise,
  we would need to implement resource collection cloning (yuck).  And newly
  built reactors in the real world usually start empty :)

* Add explicit support for simulation branching.  Branches contain a copy of
  all simulation global state (e.g. decay period, sim duration, etc.) and all
  agent-internal state at a specific timestep all under a new simulation id.
  Sim-global and agent-internal state may be modified in the copy.  The
  simulation will then initialize and restart using the potentially modified
  parameters under the new (branched) simulation id in conjunction with the
  resource inventories and other core simulation state under the parent
  simulation id.

* eventually - add support for persisting and reloading composition decay chains


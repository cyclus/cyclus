#ifndef CYCLUS_SRC_TIMER_H_
#define CYCLUS_SRC_TIMER_H_

#include <utility>
#include <vector>

#include "context.h"
#include "exchange_manager.h"
#include "product.h"
#include "material.h"
#include "infile_tree.h"
#include "time_listener.h"
#include "comp_math.h"
#include "progress_bar.h"

class SimInitTest;

namespace cyclus {

class Agent;

/// Controls simulation timestepping and inter-timestep phases.
class Timer {
  friend class ::SimInitTest;

 public:
  Timer();
  ~Timer();

  /// Sets intial time-related parameters for the simulation.
  ///
  /// @param ctx simulation context
  void Initialize(Context* ctx, SimInfo si);

  /// resets all data (registered listeners, etc.) to empty or initial state
  void Reset();

  /// Runs the simulation.
  void RunSim();

  /// Registers an agent to receive tick/tock notifications every timestep.
  /// Agents should register from their Deploy method.
  void RegisterTimeListener(TimeListener* agent);

  /// Removes an agent from receiving tick/tock notifications.
  /// Agents should unregister from their Decommission method.
  void UnregisterTimeListener(TimeListener* tl);

  /// Schedules the named prototype to be built for the specified parent at
  /// timestep t.
  void SchedBuild(Agent* parent, std::string proto_name, int t);

  /// Schedules the given Agent to be decommissioned at the specified
  /// timestep t.
  void SchedDecom(Agent* m, int time);

  /// Schedules a snapshot of simulation state to output database to occur at
  /// the beginning of the next timestep.
  void Snapshot() { want_snapshot_ = true; }

  /// Schedules the simulation to be terminated at the end of this timestep.
  void KillSim() { want_kill_ = true; }

  /// Returns the current time, in months since the simulation started.
  ///
  /// @return the current time
  int time();

  /// Returns the duration of the simulation this Timer's timing.
  ///
  /// @return the duration, in months
  int dur();

 private:
  /// builds all agents queued for the current timestep.
  void DoBuild();

  /// sends the tick signal to all of the agents receiving time
  /// notifications.
  void DoTick();

  /// Runs the resource exchange process for all traders.
  void DoResEx(ExchangeManager<Material>* matmgr,
               ExchangeManager<Product>* genmgr);

  /// sends the tock signal to all of the agents receiving time
  /// notifications.
  void DoTock();

  /// sends the decision signal to all agents recieving time
  /// notifications.
  void DoDecision();

  void RecordInventories(Agent* a);
  void RecordInventory(Agent* a, std::string name, Material::Ptr m);

  /// decommissions all agents queued for the current timestep.
  void DoDecom();

  Context* ctx_;

  /// The current time, measured in months from when the simulation
  /// started.
  int time_;

  SimInfo si_;

  bool want_snapshot_;
  bool want_kill_;

  /// Concrete agents that desire to receive tick and tock notifications
  std::map<int, TimeListener*> tickers_;
  /// The union of these two vectors should produce tickers_.
  /// Keeping C++ and Python agents separate helps support parallelization.
  std::vector<TimeListener*> cpp_tickers_;
  std::vector<TimeListener*> py_tickers_;

  // std::map<time,std::vector<std::pair<prototype, parent> > >
  std::map<int, std::vector<std::pair<std::string, Agent*>>> build_queue_;

  // std::map<time,std::vector<config> >
  std::map<int, std::vector<Agent*>> decom_queue_;

  /// Progress bar for simulation progress
  ProgressBar* progress_bar_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_TIMER_H_

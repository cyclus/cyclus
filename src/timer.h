// timer.h
#ifndef CYCLUS_TIMER_H_
#define CYCLUS_TIMER_H_

#include <utility>
#include <vector>

#include "context.h"
#include "exchange_manager.h"
#include "generic_resource.h"
#include "material.h"
#include "query_engine.h"
#include "time_listener.h"

namespace cyclus {

class Model;

/// Controls simulation timestepping and inter-timestep phases.
class Timer {
 public:

  Timer();

  /// Sets intial time-related parameters for the simulation.
  ///
  /// @param ctx simulation context
  /// @param dur duration of this simulation, in months
  /// @param m0 month the simulation starts: Jan. = 1, ..., Dec. = 12
  /// (default = 1)
  /// @param y0 year the simulation starts (default = 2010)
  /// @param start GENIUS time representing the first month of the
  /// simulation (default = 0)
  /// @param decay interval between decay calculations in months. <=0 if
  /// decay is off (default = 0)
  /// @param handle a user specified name for the simulation run
  void Initialize(Context* ctx, int dur = 1, int m0 = 1, int y0 = 2010,
                  int decay = 0, std::string handle = "");

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
  void SchedBuild(Model* parent, std::string proto_name, int t);

  /// Schedules the given Model to be decommissioned at the specified
  /// timestep t.
  void SchedDecom(Model* m, int time);

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

  /// sends the tick signal to all of the models receiving time
  /// notifications.
  void DoTick();

  /// Runs the resource exchange process for all traders.
  void DoResEx(ExchangeManager<Material>* matmgr,
               ExchangeManager<GenericResource>* genmgr);

  /// sends the tock signal to all of the models receiving time
  /// notifications.
  void DoTock();

  /// decommissions all agents queued for the current timestep.
  void DoDecom();

  Context* ctx_;

  /// The current time, measured in months from when the simulation
  /// started.
  int time_;

  /// The duration of this simulation, in months.
  int dur_;

  /// time steps between automated global material decay driving
  int decay_interval_;

  /// The number of the month (Jan = 1, etc.) corresponding to t = 0 for
  /// the scenario being run.
  int month0_;

  /// The year corresponding to t = 0 for the scenario being run.
  int year0_;

  /// Concrete models that desire to receive tick and tock notifications
  std::set<TimeListener*> tickers_;

  // std::map<time,std::vector<std::pair<prototype, parent> > >
  std::map<int, std::vector<std::pair<std::string, Model*> > > build_queue_;

  // std::map<time,std::vector<agent> >
  std::map<int, std::vector<Model*> > decom_queue_;
};

} // namespace cyclus

#endif // ifndef CYCLUS_TIMER_H_



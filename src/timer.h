#ifndef CYCLUS_SRC_TIMER_H_
#define CYCLUS_SRC_TIMER_H_

#include <utility>
#include <vector>
#include <memory>

#include "context.h"
#include "exchange_manager.h"
#include "product.h"
#include "material.h"
#include "infile_tree.h"
#include "time_listener.h"
#include "comp_math.h"
#include "indicators.hpp"

class SimInitTest;

namespace cyclus {

class Agent;

/// Controls simulation timestepping and inter-timestep phases.
class Timer {
  friend class ::SimInitTest;

 public:
  Timer() = default;
  ~Timer() = default;
  Timer(const Timer& other);
  Timer& operator=(const Timer& other);

  /// Sets intial time-related parameters for the simulation.
  ///
  /// @param ctx simulation context
  void Initialize(Context* ctx, SimInfo si);

  /// resets all data (registered listeners, etc.) to empty or initial state
  void Reset();

  /// Runs the simulation.
  void RunSim();

  /// Sets quiet mode, suppressing log output during RunSim (errors still printed).
  void SetQuiet(bool quiet) { quiet_ = quiet; }
  
  /// Returns whether quiet mode is enabled.
  bool IsQuiet() const { return quiet_; }

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

  /// Returns the number of time steps between the simulation start
  /// time and another time stamp.  
  ///
  /// @param year of alternate time stamp (must be greater than 0)
  /// @param month of alternate time stamp (must be greater than 0)
  ///
  /// @return the difference between the simulation start time and another time
  int CalcTimeDiff(int year, int month);

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

  /// @brief Determines whether or not to print the progress bar
  /// @return false if CYCLUS_PROGRESS_BAR is set to 0, false, no, or off;
  /// otherwise false when log verbosity is greater than LEV_WARN.
  bool ProgressBarEnabled();

  /// @brief Sets progress tracking state and creates the progress bar if
  /// enabled.
  void SetupProgressBar();

  /// @brief Determines whether the current completed timestep should be printed.
  bool ShouldUpdateProgressBar(int completed_steps);

  /// @brief Redraws the progress bar at the requested completed timestep.
  void RedrawProgressBar();

  /// @brief Defines how often to "update" the progress bar's progress
  /// @param duration duration of the simulation
  /// @return how many timesteps to wait between updates to the bar
  int ProgressUpdateFrequency(int duration); 

  /// @brief clamps progress to 0 <= progress <= progress_span_ and 
  /// converts to size_t for the indicators API requirement.
  /// @param completed_steps current progress as an int
  /// @return current progress as a size_t
  size_t ProgressValue(int completed_steps);


  Context* ctx_ = nullptr;

  /// The current time, measured in months from when the simulation
  /// started.
  int time_ = 0;

  SimInfo si_{0};

  bool want_snapshot_ = false;
  bool want_kill_ = false;

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
  std::unique_ptr<indicators::ProgressBar> progress_bar_ = nullptr;
  int progress_update_frequency_ = 1;
  /// First timestep index in this run (0, or branch_time when restarting).
  int progress_origin_ = 0;
  /// Number of timesteps this run will execute (si_.duration - progress_origin_).
  int progress_span_ = 1;

  bool quiet_ = false;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_TIMER_H_

#include "platform.h"
// Implements the Timer class
#include "timer.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#if CYCLUS_IS_PARALLEL
#include <omp.h>
#endif  // CYCLUS_IS_PARALLEL

#include "agent.h"
#include "error.h"
#include "logger.h"
#include "pyhooks.h"
#include "sim_init.h"

namespace cyclus {

Timer::Timer(const Timer& other) {
  *this = other;
}

Timer& Timer::operator=(const Timer& other) {
  if (this != &other) {
    ctx_ = other.ctx_;
    time_ = other.time_;
    si_ = other.si_;
    want_snapshot_ = other.want_snapshot_;
    want_kill_ = other.want_kill_;
    tickers_ = other.tickers_;
    cpp_tickers_ = other.cpp_tickers_;
    py_tickers_ = other.py_tickers_;
    build_queue_ = other.build_queue_;
    decom_queue_ = other.decom_queue_;
    progress_bar_.reset();
    progress_update_frequency_ = other.progress_update_frequency_;
    progress_origin_ = other.progress_origin_;
    progress_span_ = other.progress_span_;
    quiet_ = other.quiet_;
  }
  return *this;
}

void Timer::RunSim() {
  LogLevel saved_level = Logger::ReportLevel();
  if (quiet_) {
    // Set log level below LEV_ERROR (lowest level) to suppress all CLOG output
    Logger::SetReportLevel(static_cast<LogLevel>(-1));
  }

  CLOG(LEV_INFO1) << "Simulation set to run from start=" << 0
                  << " to end=" << si_.duration;
  CLOG(LEV_INFO1) << "Beginning simulation";

  ExchangeManager<Material> matl_manager(ctx_);
  ExchangeManager<Product> genrsrc_manager(ctx_);

  if (!progress_bar_ && ProgressBarEnabled()) {
    SetupProgressBar();
  }

  while (time_ < si_.duration) {
    CLOG(LEV_INFO1) << "Current time: " << time_;

    if (want_snapshot_) {
      want_snapshot_ = false;
      SimInit::Snapshot(ctx_);
    }

    // run through phases
    DoBuild();
    CLOG(LEV_INFO2) << "Beginning Tick for time: " << time_;
    DoTick();
    CLOG(LEV_INFO2) << "Beginning DRE for time: " << time_;
    DoResEx(&matl_manager, &genrsrc_manager);
    CLOG(LEV_INFO2) << "Beginning Tock for time: " << time_;
    DoTock();
    CLOG(LEV_INFO2) << "Beginning Decision for time: " << time_;
    DoDecision();
    DoDecom();

#ifdef CYCLUS_WITH_PYTHON
    EventLoop();
#endif

    time_++;
    RedrawProgressBar();


    if (want_kill_) {
      break;
    }
  }

  ctx_->NewDatum("Finish")
      ->AddVal("EarlyTerm", want_kill_)
      ->AddVal("EndTime", time_ - 1)
      ->Record();

  SimInit::Snapshot(
      ctx_);  // always do a snapshot at the end of every simulation

  if (quiet_) {
    Logger::SetReportLevel(saved_level);
  }
}

void Timer::DoBuild() {
  // build queued agents
  std::vector<std::pair<std::string, Agent*>> build_list = build_queue_[time_];
  for (int i = 0; i < build_list.size(); ++i) {
    Agent* m = ctx_->CreateAgent<Agent>(build_list[i].first);
    Agent* parent = build_list[i].second;
    CLOG(LEV_INFO3) << "Building a " << build_list[i].first << " from parent "
                    << build_list[i].second;
    m->Build(parent);
    if (parent != NULL) {
      parent->BuildNotify(m);
    } else {
      CLOG(LEV_DEBUG1) << "Hey! Listen! Built an Agent without a Parent.";
    }
  }
}

void Timer::DoTick() {
  for (TimeListener* agent : py_tickers_) {
    agent->Tick();
  }

#pragma omp parallel for
  for (size_t i = 0; i < cpp_tickers_.size(); ++i) {
    cpp_tickers_[i]->Tick();
  }
}

void Timer::DoResEx(ExchangeManager<Material>* matmgr,
                    ExchangeManager<Product>* genmgr) {
  matmgr->Execute();
  genmgr->Execute();
}

void Timer::DoTock() {
  for (TimeListener* agent : py_tickers_) {
    agent->Tock();
  }

#pragma omp parallel for
  for (size_t i = 0; i < cpp_tickers_.size(); ++i) {
    cpp_tickers_[i]->Tock();
  }

  if (si_.explicit_inventory || si_.explicit_inventory_compact) {
    std::set<Agent*> ags = ctx_->agent_list_;
    std::vector<Agent*> agent_vec(ags.begin(), ags.end());
#pragma omp parallel for
    for (int i = 0; i < agent_vec.size(); i++) {
      Agent* a = agent_vec[i];
      if (a->enter_time() != -1) {
        RecordInventories(a);
      }
    }
  }
}

void Timer::DoDecision() {
  for (std::map<int, TimeListener*>::iterator agent = tickers_.begin();
       agent != tickers_.end();
       agent++) {
    agent->second->Decision();
  }
}

void Timer::RecordInventories(Agent* a) {
  Inventories invs = a->SnapshotInv();
  Inventories::iterator it2;
  for (it2 = invs.begin(); it2 != invs.end(); ++it2) {
    std::string name = it2->first;
    std::vector<Resource::Ptr> mats = it2->second;
    if (mats.empty() || ResCast<Material>(mats[0]) == NULL) {
      continue;  // skip non-material inventories
    }

    Material::Ptr m = ResCast<Material>(mats[0]->Clone());
    for (int i = 1; i < mats.size(); i++) {
      m->Absorb(ResCast<Material>(mats[i]->Clone()));
    }
    RecordInventory(a, name, m);
  }
}

void Timer::RecordInventory(Agent* a, std::string name, Material::Ptr m) {
  if (si_.explicit_inventory) {
    CompMap c = m->comp()->mass();
    compmath::Normalize(&c, m->quantity());
    CompMap::iterator it;
    for (it = c.begin(); it != c.end(); ++it) {
      ctx_->NewDatum("ExplicitInventory")
          ->AddVal("AgentId", a->id())
          ->AddVal("Time", time_)
          ->AddVal("InventoryName", name)
          ->AddVal("NucId", it->first)
          ->AddVal("Quantity", it->second)
          ->AddVal("Units", m->units())
          ->Record();
    }
  }

  if (si_.explicit_inventory_compact) {
    CompMap c = m->comp()->mass();
    compmath::Normalize(&c, 1);
    ctx_->NewDatum("ExplicitInventoryCompact")
        ->AddVal("AgentId", a->id())
        ->AddVal("Time", time_)
        ->AddVal("InventoryName", name)
        ->AddVal("Quantity", m->quantity())
        ->AddVal("Units", m->units())
        ->AddVal("Composition", c)
        ->Record();
  }
}

void Timer::DoDecom() {
  // decommission queued agents
  std::vector<Agent*> decom_list = decom_queue_[time_];
  for (int i = 0; i < decom_list.size(); ++i) {
    Agent* m = decom_list[i];
    if (m->parent() != NULL) {
      m->parent()->DecomNotify(m);
    }
    m->Decommission();
  }
}

void Timer::RegisterTimeListener(TimeListener* agent) {
  tickers_[agent->id()] = agent;
  if (agent->IsShim()) {
    py_tickers_.push_back(agent);
  } else {
    cpp_tickers_.push_back(agent);
  }
}

void Timer::UnregisterTimeListener(TimeListener* tl) {
  tickers_.erase(tl->id());
  if (tl->IsShim()) {
    py_tickers_.erase(std::remove(py_tickers_.begin(), py_tickers_.end(), tl),
                      py_tickers_.end());
  } else {
    cpp_tickers_.erase(
        std::remove(cpp_tickers_.begin(), cpp_tickers_.end(), tl),
        cpp_tickers_.end());
  }
}

void Timer::SchedBuild(Agent* parent, std::string proto_name, int t) {
  if (t <= time_) {
    throw ValueError("Cannot schedule build for t < [current-time]");
  }
  build_queue_[t].push_back(std::make_pair(proto_name, parent));
}

void Timer::SchedDecom(Agent* m, int t) {
  if (t < time_) {
    throw ValueError("Cannot schedule decommission for t < [current-time]");
  }

  // It is possible that a single agent may be scheduled for decommissioning
  // multiple times. If this happens, we cannot just add it to the queue again
  // - the duplicate entries will result in a double delete attempt and
  // segfaults and otherwise bad things.  Remove previous decommissionings
  // before scheduling this new one.
  std::map<int, std::vector<Agent*>>::iterator it;
  bool done = false;
  for (it = decom_queue_.begin(); it != decom_queue_.end(); ++it) {
    int t = it->first;
    std::vector<Agent*> ags = it->second;
    for (int i = 0; i < ags.size(); i++) {
      if (ags[i] == m) {
        CLOG(LEV_WARN) << "scheduled over previous decommissioning of "
                       << m->id();
        decom_queue_[t].erase(decom_queue_[t].begin() + i);
        done = true;
        break;
      }
    }
    if (done) {
      break;
    }
  }

  decom_queue_[t].push_back(m);
}

int Timer::time() {
  return time_;
}

void Timer::Reset() {
  tickers_.clear();
  cpp_tickers_.clear();
  py_tickers_.clear();
  build_queue_.clear();
  decom_queue_.clear();
  si_ = SimInfo(0);

  progress_bar_.reset();
}

void Timer::Initialize(Context* ctx, SimInfo si) {
  if (si.m0 < 1 || si.m0 > 12) {
    throw ValueError("Invalid month0; must be between 1 and 12 (inclusive).");
  }

  want_kill_ = false;
  ctx_ = ctx;
  time_ = 0;
  si_ = si;

  if (si.branch_time > -1) {
    time_ = si.branch_time;
  }
}

bool Timer::ProgressBarEnabled() {
  const char* env_var = std::getenv("CYCLUS_PROGRESS_BAR");
  if (env_var) {
    std::string val(env_var);
    return !(val == "0" || val == "false" || val == "no" || val == "off");
  }

  // Disable with verbose logging to avoid interfering with debug output.
  return cyclus::Logger::ReportLevel() <= cyclus::LEV_WARN;
}

void Timer::SetupProgressBar() {
  progress_origin_ = time_;
  progress_span_ = std::max(si_.duration - progress_origin_, 1);

  progress_bar_.reset();
  progress_update_frequency_ = ProgressUpdateFrequency(si_.duration);
  progress_bar_.reset(new indicators::ProgressBar{
      indicators::option::BarWidth{50},
      indicators::option::MaxProgress{ProgressValue(progress_span_)},
      indicators::option::ShowPercentage{true},
  });
}

void Timer::RedrawProgressBar() {
  int completed_steps = time_ - progress_origin_;

  if (progress_bar_ &&
      (completed_steps % progress_update_frequency_ == 0 ||
      completed_steps == progress_span_)) {

    const size_t progress = ProgressValue(completed_steps);
    // Postfix must be set before set_progress: set_option does not redraw,
    // but set_progress calls print_progress() which reads postfix_text.
    progress_bar_->set_option(
        indicators::option::PostfixText{
            " (" + std::to_string(progress) + "/" +
            std::to_string(progress_span_) + ")"});
    progress_bar_->set_progress(progress);
  }
}

int Timer::ProgressUpdateFrequency(int duration) {
  return std::max(1, duration / 100);
}

size_t Timer::ProgressValue(int completed_steps) {
  return static_cast<size_t>(
      std::min(std::max(completed_steps, 0), progress_span_));
}

int Timer::dur() {
  return si_.duration;
}

int Timer::CalcTimeDiff(int year, int month) {

  int start_time = si_.y0 * cyclusYear + si_.m0 * cyclusMonth;
  int time = std::max(year,0) * cyclusYear + std::max(month,0) * cyclusMonth;

  // if time is 0, then invalid combination of year and month were given
  if (time == 0 ) {
    CLOG(LEV_WARN) << "Invalid year and month combination given to Timer::CalcTimeDiff. Returning 0. "
                   "Year: " << year << " Month: " << month;

    return 0;
  } 

  // Casting because ctx_->dt() is uint64_t and so negatives don't play nice
  return (time - start_time) / static_cast<int>(ctx_->dt());

}


}  // namespace cyclus

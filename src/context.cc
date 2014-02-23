#include "context.h"

#include <vector>

#include "error.h"
#include "exchange_solver.h"
#include "logger.h"
#include "timer.h"

namespace cyclus {

Context::Context(Timer* ti, Recorder* rec)
    : ti_(ti),
      rec_(rec),
      solver_(NULL),
      trans_id_(0) {}

Context::~Context() {
  if (solver_ != NULL) {
    delete solver_;
  }

  // initiate deletion of models that don't have parents.
  // dealloc will propagate through hierarchy as models delete their children
  std::vector<Model*> to_del;
  std::set<Model*>::iterator it;
  for (it = model_list_.begin(); it != model_list_.end(); ++it) {
    if ((*it)->parent() == NULL) {
      to_del.push_back(*it);
    }
  }
  for (int i = 0; i < to_del.size(); ++i) {
    DelModel(to_del[i]);
  }
}

void Context::DelModel(Model* m) {
  int n = model_list_.erase(m);
  if (n == 1) {
    delete m;
  }
}

boost::uuids::uuid Context::sim_id() {
  return rec_->sim_id();
}

void Context::AddPrototype(std::string name, Model* p) {
  protos_[name] = p;
}

void Context::AddRecipe(std::string name, Composition::Ptr c) {
  recipes_[name] = c;
}

Composition::Ptr Context::GetRecipe(std::string name) {
  if (recipes_.count(name) == 0) {
    throw KeyError("Invalid recipe name " + name);
  }
  return recipes_[name];
}

void Context::InitTime(int start, int duration, int decay, int m0, int y0,
                       std::string handle) {
  ti_->Initialize(this, duration, m0, y0, start, decay, handle);
}

int Context::time() {
  return ti_->time();
}

int Context::start_time() {
  return ti_->start_time();
}

int Context::sim_dur() {
  return ti_->dur();
}

void Context::RegisterTimeListener(TimeListener* tl) {
  ti_->RegisterTickListener(tl);
}

Datum* Context::NewDatum(std::string title) {
  return rec_->NewDatum(title);
}

}  // namespace cyclus

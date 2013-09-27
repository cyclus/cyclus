
#include "context.h"

#include "error.h"
#include "timer.h"

namespace cyclus {

Context::Context(Timer* ti, EventManager* em)
  : ti_(ti), em_(em) { };

boost::uuids::uuid Context::sim_id() {
  return em_->sim_id();
};

void Context::AddPrototype(std::string name, Model* p) {
  protos_[name] = p;
}

void Context::AddRecipe(std::string name, Composition::Ptr c) {
  recipes_[name] = c;
};

Composition::Ptr Context::GetRecipe(std::string name) {
  if (recipes_.count(name) == 0) {
    throw KeyError("Invalid recipe name " + name);
  }
  return recipes_[name];
};

void Context::InitTime(int start, int duration, int decay, int m0, int y0,
                       std::string handle) {
  ti_->Initialize(this, duration, m0, y0, start, decay, handle);
};

int Context::time() {
  return ti_->time();
};

int Context::start_time() {
  return ti_->start_time();
};

int Context::sim_dur() {
  return ti_->dur();
};

void Context::RegisterTicker(TimeAgent* ta) {
  ti_->RegisterTickListener(ta);
};

void Context::RegisterResolver(MarketModel* mkt) {
  ti_->RegisterResolveListener(mkt);
};

Event* Context::NewEvent(std::string title) {
  return em_->NewEvent(title);
};

} // namespace cyclus

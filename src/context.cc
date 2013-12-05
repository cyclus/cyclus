#include "error.h"
#include "exchange_solver.h"
#include "logger.h"
#include "timer.h"

#include "context.h"

namespace cyclus {

Context::Context(Timer* ti, Recorder* rec)
    : ti_(ti), rec_(rec), solver_(NULL), trans_id_(0) {};

Context::~Context() {
  if (solver_ != NULL) delete solver_;
}

boost::uuids::uuid Context::sim_id() {
  return rec_->sim_id();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Context::GetModelByName(std::string name) {
  Model* found_model = NULL;

  const std::vector<Model*>& models = model_list();
  
  for (int i = 0; i < models.size(); i++) {
    if (name == models.at(i)->name()) {
      found_model = models.at(i);
      break;
    }
  }

  if (found_model == NULL) {
    std::string err_msg = "Model '" + name + "' doesn't exist.";
    throw KeyError(err_msg);
  }
  return found_model;
}

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

Datum* Context::NewDatum(std::string title) {
  return rec_->NewDatum(title);
};

} // namespace cyclus

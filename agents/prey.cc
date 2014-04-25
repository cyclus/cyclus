#include "prey.h"
#include <time.h>

#define LG(X) LOG(cyclus::LEV_##X, "Prey")

using cyclus::ResCast;
using cyclus::Material;

Prey::Prey(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      bufsize_(0),
      birth_freq_(0),
      lifespan_(0),
      capture_prob_(0),
      for_sale_(0) {}
      // inpolicy_(this),
      // outpolicy_(this) {}

void Prey::DoRegistration() {
  cyclus::Facility::DoRegistration();
  // outpolicy_.Init(&outbuf_, outcommod_);
  // inpolicy_.Init(&inbuf_, incommod_, context()->GetRecipe(inrecipe_), 0);
  // context()->RegisterTrader(&outpolicy_);
  // context()->RegisterTrader(&inpolicy_);
  context()->RegisterTrader(this);
}

void Prey::Build(cyclus::Agent* parent) {
  cyclus::Facility::Build();
  Material::Ptr m;
  m = Material::Create(this, bufsize_, context()->GetRecipe(inrecipe_));
  inbuf_.Push(m);
}

void Prey::Decommission() {
  // context()->UnregisterTrader(&outpolicy_);
  // context()->UnregisterTrader(&inpolicy_);
  context()->UnregisterTrader(this);
  cyclus::Facility::Decommission();
}

#define LABEL "Ainimal (id=" << id() << ", proto=" << prototype() << ") "

void Prey::Tock(int t) {
  LG(INFO3) << LABEL << "is tocking";
  LG(INFO4) << "inbuf quantity = " << inbuf_.quantity();
  LG(INFO4) << "outbuf quantity = " << outbuf_.quantity();
  int age = context()->time() - enter_time();
  if (inbuf_.space() > cyclus::eps()) {
    LG(INFO3) << LABEL << "is dying of starvation";
    context()->NewDatum("LifeEvents")
        ->AddVal("AgentId", id())
        ->AddVal("Stat", "starved")
        ->Record();
    context()->SchedDecom(this);
    return;
  } else if (age >= lifespan_) {
    context()->NewDatum("LifeEvents")
        ->AddVal("AgentId", id())
        ->AddVal("Stat", "died")
        ->Record();
    LG(INFO3) << LABEL << "is dying of old age";
    context()->SchedDecom(this);
    return;
  } else if (outbuf_.quantity() < cyclus::eps() && age > 0 && for_sale_ != 0) {
    context()->NewDatum("LifeEvents")
        ->AddVal("AgentId", id())
        ->AddVal("Stat", "eaten")
        ->Record();
    LG(INFO3) << LABEL << "got eaten";
    context()->SchedDecom(this);
    return;
  }

  if (age > 0 && age % birth_freq_ == 0) {
    context()->NewDatum("LifeEvents")
        ->AddVal("AgentId", id())
        ->AddVal("Stat", "reproduced")
        ->Record();
    LG(INFO3) << LABEL << "is having 1 child";
    context()->SchedBuild(this, prototype());
  }

  for_sale_ = 0;
  outbuf_.PopN(outbuf_.count());
  cyclus::Manifest mats = inbuf_.PopN(inbuf_.count());
  double r = ((double)(rand() % 1000000)) / 1000000; // between 0 and 1
  if (r < capture_prob_ * ((double)age / lifespan_)) {
    for_sale_ = 1;
    outbuf_.PushAll(mats);
  }
}

extern "C" cyclus::Agent* ConstructPrey(cyclus::Context* ctx) {
  return new Prey(ctx);
}

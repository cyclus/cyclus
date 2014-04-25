#ifndef CYCLUS_AGENTS_PREDATOR_H_
#define CYCLUS_AGENTS_PREDATOR_H_

#include <string>
#include "cyclus.h"
// #include "buy_policy.h"
// #include "sell_policy.h"

class Predator : public cyclus::Facility {
 public:
  Predator(cyclus::Context* ctx);
  virtual ~Predator() {};
  virtual std::string str() {return "";};

  virtual void DoRegistration();
  virtual void Build(cyclus::Agent* parent = NULL);
  virtual void Decommission();

  #pragma cyclus

  virtual void Tick(int time) {};
  virtual void Tock(int time);

 private:
  /// food eaten per timestep to live
  #pragma cyclus var {'default': 1}
  double bufsize_;
  /// number of timsteps between having a single child
  #pragma cyclus var {'default': 1}
  int birth_freq_;
  /// number of timsteps until natural death
  #pragma cyclus var {'default': 4}
  int lifespan_;
  /// probability of being captured
  #pragma cyclus var {'default': 0.9}
  double capture_prob_;

  #pragma cyclus var {'default': 0}
  int for_sale_;

  #pragma cyclus var {}
  std::string incommod_;
  #pragma cyclus var {}
  std::string inrecipe_;
  #pragma cyclus var {'capacity': 'bufsize_'}
  cyclus::ResourceBuff inbuf_;

  #pragma cyclus var {}
  std::string outcommod_;
  #pragma cyclus var {'capacity': 'bufsize_'}
  cyclus::ResourceBuff outbuf_;

  // SellPolicy outpolicy_;
  // BuyPolicy inpolicy_;
};

#endif  // CYCLUS_AGENTS_PREDATOR_H_

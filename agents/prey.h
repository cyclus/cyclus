#ifndef CYCLUS_AGENTS_PREY_H_
#define CYCLUS_AGENTS_PREY_H_

#include <set>
#include <string>
#include <sstream>
#include <vector>

#include "cyclus.h"

namespace cyclus {

class Context;

/// @class Prey
/// This facility represents a Prey.
/// This facility is intended
/// to be used for Lotka-Volterra Cyclus integration tests
class Prey : public cyclus::Facility {
 public:
  Prey(cyclus::Context* ctx);
  virtual ~Prey() {}

  #pragma cyclus

  virtual void EnterNotify();
  virtual void Decommission();
  virtual void Tick(int time);
  virtual void Tock(int time);

  virtual std::set<cyclus::BidPortfolio<cyclus::Product>::Ptr>
      GetProductBids(
          cyclus::CommodMap<cyclus::Product>::type& commod_requests);

  virtual void GetProductTrades(
      const std::vector< cyclus::Trade<cyclus::Product> >& trades,
      std::vector<std::pair<cyclus::Trade<cyclus::Product>,
      cyclus::Product::Ptr> >& responses);

  inline std::string name() {
    std::stringstream ss;
    ss << prototype() << "_" << id();
    return ss.str();
  };

  void GiveBirth();
  
 private:
  #pragma cyclus var {}
  std::string commod;

  /// number of timsteps between having children
  #pragma cyclus var {'default': 1}
  int birth_freq;

  // number of children
  #pragma cyclus var {'default': 1}
  int nchildren;
  
  /// age of a prey
  #pragma cyclus var {'default': 0}
  int age;

  #pragma cyclus var {'default': 0}
  int dead;

  /// whether or not an agent can give birth and die in the same timestep
  #pragma cyclus var {'default': 1} // true
  int birth_and_death;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_PREY_H_

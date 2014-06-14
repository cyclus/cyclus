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

  #pragma cyclus note {"doc": "A facility that represents prey " \
                              "in the Lotka-Volterra integration " \
                              "tests"}

  virtual void EnterNotify();
  virtual void Decommission();
  virtual void Tick();
  virtual void Tock();

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
  #pragma cyclus var {"schematype": "token"}
  std::string commod;

  /// number of timsteps between having children
  #pragma cyclus var {'default': 1, "tooltip": "birth frequency", \
                      "doc": "number of time steps between birth " \
                             "of children"}
  int birth_freq;

  // number of children
  #pragma cyclus var {'default': 1, "tooltip": "number of children", \
                      "doc": "number of children born at each " \
                             "birthing instance"}
  int nchildren;
  
  /// age of a prey
  #pragma cyclus var {'default': 0, "tooltip": "prey age", \
                      "doc": "age of prey at start of simulation"}
  int age;

  #pragma cyclus var {'default': 0, "tooltip": "dead?", \
                      "doc": "flag for whether prey is currently dead"}
  bool dead;

  /// whether or not an agent can give birth and die in the same timestep
  #pragma cyclus var {'default': 1, "tooltip": "simultaneous birth " \
                                    "and death?", \
                      "doc": "whether or not simultaneous birth and " \
                             "and death are allowed (i.e., can a " \
                             "facility give birth and die in the " \
                             "same time step?)"} // true
  bool birth_and_death;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_PREY_H_

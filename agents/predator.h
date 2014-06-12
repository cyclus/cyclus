#ifndef CYCLUS_AGENTS_PREDATOR_H_
#define CYCLUS_AGENTS_PREDATOR_H_

#include <string>
#include <sstream>

#include "cyclus.h"

namespace cyclus {

/// @class Predator
/// This Predator facility simulates hunters on preys.
class Predator : public cyclus::Facility  {
 public:
  /// smallest first! 
  static inline bool SortById(cyclus::Bid<cyclus::Product>* l,
                              cyclus::Bid<cyclus::Product>* r) {
    return l->bidder()->manager()->id() < r->bidder()->manager()->id();
  }

  Predator(cyclus::Context* ctx);
  virtual ~Predator() {}

  #pragma cyclus

  #pragma cyclus note {"doc": "A facility that represents predators " \
                              "in the Lotka-Volterra integration tests"}

  virtual void EnterNotify();
  virtual void Decommission();
  virtual void Tick();
  virtual void Tock();

  /// @brief Predator request Product of their given
  /// commodity. Note that it is assumed the Predator operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
      GetProductRequests();

  virtual void AdjustProductPrefs(cyclus::PrefMap<cyclus::Product>::type& prefs);

  /// @brief Predator place accepted trade Materials in their Inventory
  virtual void AcceptProductTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
      cyclus::Product::Ptr> >& responses);

  inline std::string name() {
    std::stringstream ss;
    ss << prototype() << "_" << id();
    return ss.str();
  };

  void GiveBirth();
  
 private:
  #pragma cyclus var {"tooltip": "predator commodity", \
                      "doc": "commodity that the predator supplies", \
                      "schematype": "token"}
  std::string commod;

  #pragma cyclus var {"tooltip": "predator's prey", \
                      "doc": "prey that the predator hunts", \
                      "schematype": "token"}
  std::string prey;

  /// how many prey until we're full
  #pragma cyclus var {'default': 1, "tooltip": "feast size", \
                      "doc": "how many units of prey a predator " \
                             "consumes until it is satisfied"}
  double full;
  
  /// how many prey we can catch on the hunt
  #pragma cyclus var {'default': 1, "tooltip": "hunting yield", \
                      "doc": "how many units of prey a predator " \
                             "can catch during a hunt"}
  double hunt_cap;
  
  /// how often we hunt
  #pragma cyclus var {'default': 1, "tooltip": "hunting frequency", \
                      "doc": "how often a predator needs to hunt"}
  int hunt_freq;

  /// hunting success on a scale from 1 to 0
  #pragma cyclus var {'default': 1, "tooltip": "hunting success fraction", \
                      "doc": "fraction of hunting success on a scale " \
                             "from 0 to 1"}
  double success;

  #pragma cyclus var {'default': 1, "tooltip": "number of children", \
                      "doc": "number of predator children born at " \
                             "each birthing instance"}
  double nchildren;

  /// age of a predator
  #pragma cyclus var {'default': 0, "tooltip": "predator age", \
                      "doc": "age of predator at beginning of simulation"}
  int age;

  #pragma cyclus var {'default': 1, "tooltip": "predator lifespan", \
                      "doc": "how long a predator lives"}
  int lifespan;

  #pragma cyclus var {'default': 0, "tooltip": "dead?", \
                      "doc": "flag for whether predator is currently dead"}
  bool dead;

  /// whether or not to base hunt succes on relative prey/predator populations
  #pragma cyclus var {'default': 0, "tooltip": "hunting success factor", \
                      "doc": "whether or not to base hunting success on " \
                             "relative predator/prey populations"} // false
  bool hunt_factor;

  /// whether or not an agent can give birth and die in the same timestep  
  #pragma cyclus var {'default': 0, "tooltip": "simultaneous birth " \
                                               "and death?", \
                      "doc": "whether or not simultaneous birth and " \
                             "and death are allowed (i.e., can a " \
                             "facility give birth and die in the " \
                             "same time step?)"} // false
  bool birth_and_death;

  /// consumption this time step
  #pragma cyclus var {'default': 0, "tooltip": "prey consumed", \
                      "doc": "how many units of prey consumed per time step"}
  double consumed;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_PREDATOR_H_

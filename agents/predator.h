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

  virtual void EnterNotify();
  virtual void Decommission();
  virtual void Tick(int time);
  virtual void Tock(int time);

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
  #pragma cyclus var {}
  std::string commod_;

  #pragma cyclus var {}
  std::string prey_;

  /// how many prey until we're full
  #pragma cyclus var {'default': 1}
  double full_;
  
  /// how many prey we can catch on the hunt
  #pragma cyclus var {'default': 1}
  double hunt_cap_;
  
  /// how often we hunt
  #pragma cyclus var {'default': 1}
  int hunt_freq_;

  /// hunting success on a scale from 1 to 0
  #pragma cyclus var {'default': 1}
  double success_;

  #pragma cyclus var {'default': 1}
  double nchildren_;

  /// age of a prey
  #pragma cyclus var {'default': 0}
  int age_;

  #pragma cyclus var {'default': 1}
  int lifespan_;

  #pragma cyclus var {'default': 0}
  int dead_;

  /// whether or not to base hunt succes on relative prey/predator populations
  #pragma cyclus var {'default': 0} // false
  int hunt_factor_;

  /// whether or not an agent can give birth and die in the same timestep  
  #pragma cyclus var {'default': 0} // false
  int birth_and_death_;

  /// consumption this time step
  #pragma cyclus var {'default': 0}
  double consumed_;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_PREDATOR_H_

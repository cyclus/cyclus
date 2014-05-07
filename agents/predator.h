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
  Predator(cyclus::Context* ctx);
  virtual ~Predator() {}

  #pragma cyclus

  virtual std::string str();
  virtual void DoRegistration();
  virtual void Build(cyclus::Agent* parent = NULL);
  virtual void Decommission();
  virtual void Tick(int time);
  virtual void Tock(int time);

  /// @brief Predator request Product of their given
  /// commodity. Note that it is assumed the Predator operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
      GetProductRequests();

  /// @brief Predator place accepted trade Materials in their Inventory
  virtual void AcceptProductTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
      cyclus::Product::Ptr> >& responses);

  /// @brief determines the amount to request
  double capacity();

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

  /**
    Capacity is the number of members in the predator.
    The default represents only one entity.
    It may also be modeled as a group of several predators.
   */
  #pragma cyclus var {'default': 1}
  double capacity_;

  /// hunting success
  #pragma cyclus var {'default': 0.1}
  double success_;

  // efficiency of converting food into children
  #pragma cyclus var {'default': 1}
  double birth_factor_;

  /// age of a prey
  #pragma cyclus var {'default': 0}
  int age_;

  #pragma cyclus var {'default': 1}
  int lifespan_;

  #pragma cyclus var {'default': 0}
  int dead_;

  /// whether or not an agent can give birth and die in the same timestep  
  #pragma cyclus var {'default': 1} // true
  int birth_and_death_;

  /// consumption this time step
  #pragma cyclus var {'default': 0}
  double consumed_;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_PREDATOR_H_

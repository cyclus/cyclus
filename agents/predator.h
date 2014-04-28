#ifndef CYCLUS_AGENTS_PREDATOR_H_
#define CYCLUS_AGENTS_PREDATOR_H_

#include <string>

#include "cyclus.h"
#include "prey.h"

// extern int g_nPrey;

namespace cyclus {

/// @class Predator
/// This Predator facility simulates hunters on preys.
class Predator : public cyclus::Facility  {
 public:
  Predator(cyclus::Context* ctx);
  virtual ~Predator() {}

  #pragma cyclus clone
  #pragma cyclus initfromcopy
  #pragma cyclus initfromdb
  #pragma cyclus infiletodb
  #pragma cyclus snapshot
  #pragma cyclus schema

  virtual void InitInv(cyclus::Inventories& inv);

  virtual cyclus::Inventories SnapshotInv();

  virtual void DoRegistration();
  virtual void Build(cyclus::Agent* parent = NULL);
  virtual void Decommission();

  virtual std::string str();

  virtual void Tick(int time);

  virtual void Tock(int time);

  /// @brief Predator request Materials of their given commodity. Note
  /// that it is assumed the Predator operates on a single resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests();

  /// @brief Predator request Product of their given
  /// commodity. Note that it is assumed the Predator operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
      GetGenRsrcRequests();

  /// @brief Predator place accepted trade Materials in their Inventory
  virtual void AcceptMatlTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
      cyclus::Material::Ptr> >& responses);

  /// @brief Predator place accepted trade Materials in their Inventory
  virtual void AcceptGenRsrcTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
      cyclus::Product::Ptr> >& responses);

  /// @brief determines the amount to request
  inline double capacity() const {
    std::cout << cyclus::g_nPrey << std::endl;
    return success_ * g_nPrey; }

 private:
  cyclus::ResourceBuff inventory_;

  #pragma cyclus var {}
  std::string commod_;

  #pragma cyclus var {}
  std::string recipe_;

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

  #pragma cyclus var {'default': 12}
  int lifespan_;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_PREDATOR_H_

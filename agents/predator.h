#ifndef CYCLUS_AGENTS_PREDATOR_H_
#define CYCLUS_AGENTS_PREDATOR_H_

#include <string>

#include "cyclus.h"

namespace cyclus {

/// @class Predator
/// This Predator facility accepts specified amount of commodity.
/// This Predator facility is similar to Sink provided in cycamore, but it
/// has minimum implementation to run integration tests.
/// Some parts of the code is directrly copied from cycamore Sink.
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
  inline double capacity() const { return capacity_; }

 private:
  #pragma cyclus var {}
  std::vector<std::string> in_commods_;

  #pragma cyclus var {}
  double capacity_;

  cyclus::ResourceBuff inventory_;

  /// food eaten per timestep to live
  #pragma cyclus var {'default': 1}
  double bufsize_;
  /// number of timsteps between having a single child
  #pragma cyclus var {'default': 1}
  int birth_freq_;
  /// age
  #pragma cyclus var {'default': 4}
  int age_;
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
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_PREDATOR_H_

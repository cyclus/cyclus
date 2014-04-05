#ifndef CYCLUS_AGENTS_SINK_H_
#define CYCLUS_AGENTS_SINK_H_

#include <string>

#include "cyclus.h"

namespace cyclus {

/// @class Sink
/// This sink facility accepts specified amount of commodity.
/// This sink facility is similar to Sink provided in cycamore, but it
/// has minimum implementation to run integration tests.
/// Some parts of the code is directrly copied from cycamore Sink.
class Sink : public cyclus::Facility  {
 public:
  Sink(cyclus::Context* ctx);
  virtual ~Sink() {};

  #pragma cyclus clone
  #pragma cyclus initfromcopy
  #pragma cyclus initfromdb
  #pragma cyclus infiletodb
  #pragma cyclus snapshot
  #pragma cyclus schema

  virtual void InitInv(cyclus::Inventories& inv);

  virtual cyclus::Inventories SnapshotInv();

  virtual std::string str();

  virtual void Tick(int time);

  virtual void Tock(int time);

  /// @brief Sink request Materials of their given commodity. Note
  /// that it is assumed the Sink operates on a single resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests();

  /// @brief Sink request Product of their given
  /// commodity. Note that it is assumed the Sink operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
      GetGenRsrcRequests();

  /// @brief Sink place accepted trade Materials in their Inventory
  virtual void AcceptMatlTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
      cyclus::Material::Ptr> >& responses);

  /// @brief Sink place accepted trade Materials in their Inventory
  virtual void AcceptGenRsrcTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
      cyclus::Product::Ptr> >& responses);
  /**
     add a commodity to the set of input commodities
     @param name the commodity name
   */
  inline void AddCommodity(std::string name) {
    in_commods_.push_back(name);
  }

  /// determines the amount to request
  inline double capacity() const { return capacity_; }

 private:
  #pragma cyclus var {}
  std::vector<std::string> in_commods_;

  #pragma cyclus var {}
  double capacity_;

  cyclus::ResourceBuff inventory_;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_SINK_H_

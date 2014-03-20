#ifndef CYCLUS_INTEGRATION_TESTS_MODELS_SINK_FACILITY_H_
#define CYCLUS_INTEGRATION_TESTS_MODELS_SINK_FACILITY_H_

#include <string>

#include "cyclus.h"

namespace cyclus {

/// @class Sink
/// This sink facility accepts specified amount of commodity.
/// This sink facility is similar to Sink provided in cycamore, but it
/// has minimum implementation to run integration tests.
/// Some parts of the code is directrly copied from cycamore Sink.
class Sink : public cyclus::FacilityAgent  {
 public:
  Sink(cyclus::Context* ctx);
  virtual ~Sink() {};

  #pragma cyclus

  virtual void InitInv(cyc::Inventories& inv);
  
  virtual cyc::Inventories SnapshotInv();

  virtual std::string str();

  virtual void Tick(int time);

  virtual void Tock(int time);

  /// @brief SinkFacilities request Materials of their given commodity. Note
  /// that it is assumed the Sink operates on a single resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests();

  /// @brief SinkFacilities request GenericResources of their given
  /// commodity. Note that it is assumed the Sink operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::GenericResource>::Ptr>
      GetGenRsrcRequests();

  /// @brief SinkFacilities place accepted trade Materials in their Inventory
  virtual void AcceptMatlTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
      cyclus::Material::Ptr> >& responses);

  /// @brief SinkFacilities place accepted trade Materials in their Inventory
  virtual void AcceptGenRsrcTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::GenericResource>,
      cyclus::GenericResource::Ptr> >& responses);
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
  std::vector<std::string> in_commods_;
  std::string incommodity_;
  double capacity_;
  cyclus::ResourceBuff inventory_;
};

}  // namespace cyclus

#endif  // CYCLUS_INTEGRATION_TESTS_MODELS_SINK_FACILITY_H_

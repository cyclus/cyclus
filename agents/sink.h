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
  virtual ~Sink() {}

  #pragma cyclus

  #pragma cyclus note {"doc": "A minimum implementation sink " \
                              "facility that accepts specified " \
                              "amounts of commodities from " \
                              "other agents"}

  virtual std::string str();

  virtual void Tick();

  virtual void Tock();

  /// @brief Sink request Materials of their given commodity. Note
  /// that it is assumed the Sink operates on a single resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests();

  /// @brief Sink request Product of their given
  /// commodity. Note that it is assumed the Sink operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
      GetProductRequests();

  /// @brief Sink place accepted trade Materials in their Inventory
  virtual void AcceptMatlTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
      cyclus::Material::Ptr> >& responses);

  /// @brief Sink place accepted trade Materials in their Inventory
  virtual void AcceptProductTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
      cyclus::Product::Ptr> >& responses);

  /// @brief determines the amount to request
  inline double Capacity() const { return capacity; }

 private:
  #pragma cyclus var {"doc": "commodities that the sink facility " \
                             "accepts", \
                      "tooltip": "input commodities"}
  std::vector<std::string> in_commods;

  #pragma cyclus var {"doc": "capacity the sink facility can " \
                             "accept at each time step", \
                      "tooltip": "sink capacity"}
  double capacity;

  #pragma cyclus var {"default": 1e299, \
                      "doc": "total maximum inventory size of " \
                             "sink facility", \
                      "tooltip": "sink maximum inventory size"}
  double max_inv_size;

  #pragma cyclus var {'capacity': 'max_inv_size'}
  cyclus::toolkit::ResourceBuff inventory;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_SINK_H_

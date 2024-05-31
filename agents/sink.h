#ifndef CYCLUS_AGENTS_SINK_H_
#define CYCLUS_AGENTS_SINK_H_

#include <string>

#include "cyclus.h"

#pragma cyclus exec from sidecar import CY_LARGE_DOUBLE, CY_LARGE_INT, CY_NEAR_ZERO

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

  virtual std::string version() { return cyclus::version::describe(); }

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

  inline void Capacity(double cap) { capacity = cap; }

  void AddIncommod(std::string commod) {in_commods.push_back(commod);};

  /// sets the name of the recipe to be requested
  inline void recipe(std::string name) { recipe_name = name; }

  /// the name of the input recipe to request
  inline std::string recipe() const { return recipe_name; }

  virtual void Build(cyclus::Agent* parent) {
    Facility::Build(parent);
    if (lifetime() >= 0) {
      context()->SchedDecom(this, exit_time());
    }
  }

 private:
  #pragma cyclus var {\
    "doc": "commodities that the sink facility accepts ",	   \
    "tooltip": "input commodities for the sink",		   \
    "uilabel": "List of Input Commodities",			   \
    "uitype": ["oneormore", "incommodity"]			   \
  }
  std::vector<std::string> in_commods;

  #pragma cyclus var { \
    "tooltip": "input/request recipe name", \
    "doc": "Name of recipe to request." \
           "If empty, sink requests material no particular composition.", \
    "default": "", \
    "uilabel": "Input Recipe",			\
    "uitype": "recipe", \
  }
  std::string recipe_name;

  #pragma cyclus var {\
    "default": CY_LARGE_DOUBLE,						\
    "doc": "total maximum inventory size of "			\
           "sink facility",						\
    "uilabel": "Maximum Inventory",				\
    "tooltip": "sink maximum inventory size" \
  }
  double max_inv_size;

  #pragma cyclus var { \
    "doc": "capacity the sink facility can " \
           "accept at each time step", \
    "uilabel": "Maximum Throughput", \
    "uitype": "range", \
    "range": [0.0, CY_LARGE_DOUBLE], \
    "tooltip": "sink capacity" \
  }
  double capacity;

  #pragma cyclus var {'capacity': 'max_inv_size'}
  cyclus::toolkit::ResBuf<cyclus::Resource> inventory;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_SINK_H_

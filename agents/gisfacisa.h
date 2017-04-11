#ifndef CYCLUS_AGENTS_GISFACISA_H_
#define CYCLUS_AGENTS_GISFACISA_H_

#include <string>

#include "cyclus.h"
#include "gis.h"

namespace cyclus {

/// @class GISFacisa
/// This gisfacisa facility accepts specified amount of commodity.
/// This gisfacisa facility is similar to GISFacisa provided in cycamore, but it
/// has minimum implementation to run integration tests.
/// Some parts of the code is directrly copied from cycamore GISFacisa.
class GISFacisa : public cyclus::Facility, public cyclus::GIS {
 public:
  GISFacisa(cyclus::Context* ctx);
  GISFacisa(cyclus::Context* ctx, float decimal_lat, float decimal_lon);
  virtual ~GISFacisa() {}

  virtual std::string version() { return cyclus::version::describe(); }

#pragma cyclus

#pragma cyclus note {"doc": "A minimum implementation gisfacisa " \
                              "facility that accepts specified " \
                              "amounts of commodities from " \
                              "other agents"}

  virtual std::string str();

  virtual void Tick();

  virtual void Tock();

  /// @brief GISFacisa request Materials of their given commodity. Note
  /// that it is assumed the GISFacisa operates on a single resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
  GetMatlRequests();

  /// @brief GISFacisa request Product of their given
  /// commodity. Note that it is assumed the GISFacisa operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
  GetProductRequests();

  /// @brief GISFacisa place accepted trade Materials in their Inventory
  virtual void AcceptMatlTrades(
      const std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                                  cyclus::Material::Ptr> >& responses);

  /// @brief GISFacisa place accepted trade Materials in their Inventory
  virtual void AcceptProductTrades(
      const std::vector<std::pair<cyclus::Trade<cyclus::Product>,
                                  cyclus::Product::Ptr> >& responses);

  /// @brief determines the amount to request
  inline double Capacity() const { return capacity; }

  inline void Capacity(double cap) { capacity = cap; }

  void AddIncommod(std::string commod) { in_commods.push_back(commod); };

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
#pragma cyclus var {                                          \
  "doc" : "commodities that the gisfacisa facility accepts ", \
  "tooltip" : "input commodities for the gisfacisa",          \
  "uilabel" : "List of Input Commodities",                    \
  "uitype" : ["oneormore", "incommodity"] }
  std::vector<std::string> in_commods;

#pragma cyclus var { \
    "tooltip": "input/request recipe name", \
    "doc": "Name of recipe to request." \
           "If empty, gisfacisa requests material no particular composition.", \
    "default": "", \
    "uilabel": "Input Recipe",      \
    "uitype": "recipe", \
  }
  std::string recipe_name;

#pragma cyclus var {\
    "default": 1e299,           \
    "doc": "total maximum inventory size of "     \
           "gisfacisa facility",           \
    "uilabel": "Maximum Inventory",       \
    "tooltip": "gisfacisa maximum inventory size" \
  }
  double max_inv_size;

#pragma cyclus var { \
    "doc": "capacity the gisfacisa facility can "     \
           "accept at each time step",         \
    "uilabel": "Maximum Throughput",       \
    "tooltip": "gisfacisa capacity"  \
  }
  double capacity;

#pragma cyclus var { 'capacity' : 'max_inv_size' }
  cyclus::toolkit::ResourceBuff inventory;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_GISFACISA_H_

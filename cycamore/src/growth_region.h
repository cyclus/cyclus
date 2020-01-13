#ifndef CYCAMORE_SRC_GROWTH_REGION_H_
#define CYCAMORE_SRC_GROWTH_REGION_H_

#include <string>
#include <utility>
#include <vector>

#include "cyclus.h"
#include "cycamore_version.h"

// forward declarations
namespace cycamore {
class GrowthRegion;
}  // namespace cycamore

// forward includes
#include "growth_region_tests.h"

namespace cycamore {

/// A container of (time, (demand type, demand parameters))
typedef std::vector<
  std::pair<int, std::pair<std::string, std::string> > > Demand;

/// This region determines if there is a need to meet a certain
/// capacity (as defined via input) at each time step. If there is
/// such a need, the region will determine how many of each facility
/// type are required and then determine, facility by facility, which
/// of its institutions are available to build each facility.
///
/// @TODO In order to make GrowthRegion copacetic with init/restart, its input
/// parameter space was simplified. For now it can only provide growth support
/// for a single demanded commodity. A relatively simple next step will be to
/// determin an input API and corresponding implementation that again supports
/// multiple commodities being demanded.
///
/// @warning The growth region is experimental
class GrowthRegion : public cyclus::Region,
  public cyclus::toolkit::Position {
  friend class GrowthRegionTests;
 public:
  /// The default constructor for the GrowthRegion
  GrowthRegion(cyclus::Context* ctx);

  /// The default destructor for the GrowthRegion
  virtual ~GrowthRegion();

  virtual std::string version() { return CYCAMORE_VERSION; }

  #pragma cyclus

  #pragma cyclus note {"doc": "A region that governs a scenario in which " \
                              "there is growth in demand for a commodity. "}

  /// On each tick, the GrowthRegion queries its supply demand manager
  /// to determine if there exists some demand. If demand for a
  /// commodity exists, then the correct build order for that demand
  /// is constructed and executed.
  /// @param time is the time to perform the tick
  virtual void Tick();

  /// enter the simulation and register any children present
  virtual void EnterNotify();

  /// unregister a child
  virtual void DecomNotify(Agent* m);

  inline cyclus::toolkit::SupplyDemandManager* sdmanager() {
    return &sdmanager_;
  }

 protected:
  #pragma cyclus var { \
    "alias": ["growth", "commod", \
              ["piecewise_function",                                    \
               ["piece", "start", ["function", "type", "params"]]]],    \
    "uitype": ["oneormore", "string", \
               ["oneormore", \
                ["pair", "int", ["pair", "string", "string"]]]],      \
    "uilabel": "Growth Demand Curves", \
    "doc": "Nameplate capacity demand functions." \
    "\n\n"                                       \
    "Each demand type must be for a commodity for which capacity can be built "\
    "(e.g., 'power' from cycamore::Reactors). Any archetype that implements the "\
    "cyclus::toolkit::CommodityProducer interface can interact with the "\
    "GrowthRegion in the manner."                                       \
    "\n\n"                                       \
    "Demand functions are defined as piecewise functions. Each piece must "\
    "be provided a starting time and function description. Each function "\
    "description is comprised of a function type and associated parameters. "\
    "\n\n"                                       \
    "  * Start times are inclusive. For a start time :math:`t_0`, the demand "\
    "function is evaluated on :math:`[t_0, \infty)`."                     \
    "\n\n"                                       \
    "  * Supported function types are based on the "\
    "`cyclus::toolkit::BasicFunctionFactory "\
    "types <http://fuelcycle.org/cyclus/api/classcyclus_1_1toolkit_1_1BasicFunctionFactory.html#a2f3806305d99a745ab57c300e54a603d>`_. " \
    "\n\n"                                       \
    "  * The type name is the lower-case name of the function (e.g., " \
    "'linear', 'exponential', etc.)." \
    "\n\n"                                       \
    "  * The parameters associated with each function type can be found on their " \
    "respective documentation pages.",                                  \
  }
  std::map<std::string, std::vector<std::pair<int, std::pair<std::string, std::string> > > > commodity_demand; // must match Demand typedef

#if CYCLUS_HAS_COIN
  /// manager for building things
  cyclus::toolkit::BuildingManager buildmanager_;
#endif

  /// manager for Supply and demand
  cyclus::toolkit::SupplyDemandManager sdmanager_;

  /// register a child
  void Register_(cyclus::Agent* agent);

  /// unregister a child
  void Unregister_(cyclus::Agent* agent);

  /// add a demand for a commodity on which this region request that
  /// facilities be built
  void AddCommodityDemand_(std::string commod, Demand& demand);

  /// orders builds given a commodity and an unmet demand for production
  /// capacity of that commodity
  /// @param commodity the commodity being demanded
  /// @param unmetdemand the unmet demand
  void OrderBuilds(cyclus::toolkit::Commodity& commodity, double unmetdemand);

  private:
  #pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Geographical latitude in degrees as a double", \
    "doc": "Latitude of the agent's geographical position. The value should " \
           "be expressed in degrees as a double." \
  }
  double latitude;

  #pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Geographical longitude in degrees as a double", \
    "doc": "Longitude of the agent's geographical position. The value should " \
           "be expressed in degrees as a double." \
  }
  double longitude;

  cyclus::toolkit::Position coordinates;

  /// Records an agent's latitude and longitude to the output db
  void RecordPosition();
};
}  // namespace cycamore

#endif  // CYCAMORE_SRC_GROWTH_REGION_H_

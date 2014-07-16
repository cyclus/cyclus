#ifndef CYCLUS_AGENTS_SOURCE_H_
#define CYCLUS_AGENTS_SOURCE_H_

#include <set>
#include <string>
#include <vector>

#include "cyclus.h"

namespace cyclus {

class Context;

/// @class Source
/// This facility is a source of some commodity with some capacity.
/// This facility is sort of reduced copy of cycamore's source facility.
/// A lot of implementation code is barrowed from cycamore's source facility
/// implementation files.
///
/// This Facility is intended
/// to be used for Cyclus integration tests as a basic source facility.
class Source : public cyclus::Facility {
 public:
  Source(cyclus::Context* ctx);
  virtual ~Source() {}

  #pragma cyclus

  #pragma cyclus note {"doc": "A minimum implementation source " \
                              "facility that provides a commodity " \
                              "with a given capacity"}

  virtual std::string str();

  virtual void Tick();

  virtual void Tock();

  /// @brief Responds to each request for this source facility's commodity.
  /// If a given request is more than this facility's capacity, it will offer
  /// its capacity.
  virtual std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
      GetMatlBids(cyclus::CommodMap<cyclus::Material>::type&
                  commod_requests);

  /// @brief respond to each trade with a material made from this facility's
  /// recipe
  ///
  /// @param trades all trades in which this trader is the supplier
  /// @param responses a container to populate with responses to each trade
  virtual void GetMatlTrades(
      const std::vector< cyclus::Trade<cyclus::Material> >& trades,
      std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                            cyclus::Material::Ptr> >& responses);

  /// @brief creates a material object to offer to a requester
  /// @param target the material target a request desires
  cyclus::Material::Ptr GetOffer(const cyclus::Material::Ptr target) const;

  /// @brief sets the output commodity name
  /// @param name the commodity name
  inline void commodity(std::string name) { commod = name; }

  /// @return the output commodity
  inline std::string commodity() const { return commod; }

  /// @brief sets the capacity of a material generated at any given time step
  /// @param cap the production capacity
  inline void Capacity(double cap) { capacity = cap; }

  /// @return the production capacity at any given time step
  inline double Capacity() const { return capacity; }

  /// @brief sets the name of the recipe to be produced
  /// @param name the recipe name
  inline void recipe(std::string name) { recipe_name = name; }

  /// @return the name of the output recipe
  inline std::string recipe() const { return recipe_name; }

 private:
  #pragma cyclus var {"doc": "commodity that the source facility " \
                             "supplies", \
                      "tooltip": "source commodity", \
                      "schematype": "token"}
  std::string commod;

  #pragma cyclus var {"doc": "recipe name for source facility's " \
                             "commodity", \
                      "tooltip": "commodity recipe name", \
                      "schematype": "token"}
  std::string recipe_name;

  /// The capacity is defined in terms of the number of units of the
  /// recipe that can be provided each time step.  A very large number
  /// can be provided to represent infinte capacity.
  #pragma cyclus var {"doc": "amount of commodity that can be " \
                             "supplied at each time step", \
                      "tooltip": "source capacity"}
  double capacity;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_SOURCE_H_

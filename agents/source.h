#ifndef CYCLUS_AGENTS_SOURCE_H_
#define CYCLUS_AGENTS_SOURCE_H_

#include <string>
#include <set>
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
/// This FacilityAgent is intended
/// to be used for Cyclus integration tests as a basic source facility.
class Source : public cyclus::FacilityAgent {
 public:
  Source(cyclus::Context* ctx);
  virtual ~Source() {};

  #pragma cyclus

  virtual std::string str();

  virtual void InitInv(cyc::Inventories& inv) {};

  virtual cyclus::Inventories SnapshotInv() { return cyc::Inventories(); }

  virtual void Tick(int time);

  virtual void Tock(int time);

  /// @brief Responds to each request for this source facility's commodity.
  /// If a given request is more than this facility's capacity, it will offer
  /// its capacity.
  virtual std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
      GetMatlBids(const cyclus::CommodMap<cyclus::Material>::type&
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

  /**
    @brief creates a material object to offer to a requester
    @param target the material target a request desires
    */
  cyclus::Material::Ptr GetOffer(const cyclus::Material::Ptr target) const;

  /**
    sets the output commodity name
    @param name the commodity name
    */
  inline void commodity(std::string name) { commod_ = name; }

  /// @return the output commodity
  inline std::string commodity() const { return commod_; }

  /**
    sets the capacity of a material generated at any given time step
    @param capacity the production capacity
    */
  inline void capacity(double capacity) { capacity_ = capacity; }

  /// @return the production capacity at any given time step
  inline double capacity() const { return capacity_; }

  /**
    sets the name of the recipe to be produced
    @param name the recipe name
    */
  inline void recipe(std::string name) { recipe_name_ = name; }

  /// @return the name of the output recipe
  inline std::string recipe() const { return recipe_name_; }

 private:
  std::string commod_;

  std::string recipe_name_;

  /**
    The capacity is defined in terms of the number of units of the
    recipe that can be provided each time step.  A very large number
    can be provided to represent infinte capacity.
    */
  double capacity_;

};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_SOURCE_H_

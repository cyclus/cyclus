#ifndef CYCLUS_AGENTS_PREY_H_
#define CYCLUS_AGENTS_PREY_H_

#include <set>
#include <string>
#include <vector>

#include "cyclus.h"

namespace cyclus {

class Context;

/// @class Prey
/// This facility represents a Prey.
/// This facility is intended
/// to be used for Lotka-Volterra Cyclus integration tests
class Prey : public cyclus::Facility {
 public:
  Prey(cyclus::Context* ctx);
  virtual ~Prey() {}

  #pragma cyclus

  virtual std::string str();

  virtual void DoRegistration();
  virtual void Build(cyclus::Agent* parent = NULL);
  virtual void Decommission();

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
     @brief sets the output commodity name
     @param name the commodity name
   */
  inline void commodity(std::string name) { commod_ = name; }

  /// @return the output commodity
  inline std::string commodity() const { return commod_; }

  /**
     @brief sets the capacity of a material generated at any given time step
     @param capacity the production capacity
   */
  inline void capacity(double capacity) { capacity_ = capacity; }

  /// @return the production capacity at any given time step
  inline double capacity() const { return capacity_; }

  /**
     @brief sets the name of the recipe to be produced
     @param name the recipe name
   */
  inline void recipe(std::string name) { recipe_ = name; }

  /// @return the name of the output recipe
  inline std::string recipe() const { return recipe_; }

 private:
  #pragma cyclus var {}
  std::string commod_;

  #pragma cyclus var {}
  std::string recipe_;

  /**
    Capacity is the number of members in this prey agent. The default
    represents only one entity. It may also be modeled as a group of
    several preys.
   */
  #pragma cyclus var {'default': 1}
  double capacity_;

  #pragma cyclus var {'default': 0}
  int killed_;

  /// number of timsteps between having children
  #pragma cyclus var {'default': 1}
  int birth_freq_;
  // number of children
  #pragma cyclus var {'default': 1}
  int nchildren_;
  /// age of a prey
  #pragma cyclus var {'default': 0}
  int age_;

  #pragma cyclus var {'default': 4}
  int lifespan_;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_PREY_H_

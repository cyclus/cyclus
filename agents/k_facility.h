#ifndef CYCLUS_AGENTS_K_FACILITY_H_
#define CYCLUS_AGENTS_K_FACILITY_H_

#include <set>
#include <string>
#include <vector>

#include "cyclus.h"

namespace cyclus {

class Context;

/// @class KFacility
///
/// @section intro Introduction
/// The KFacility is a source and consumer of commodities.
/// The facility changes its request and offer amount according to a power law.
///
/// Amount = Capacity(T=0) * ConversionFactor ^ Time
///
/// This facility is intended to be used for Cyclus trivial and
/// minimal cycle integration tests.

/// @section params Parameters
/// The parameters relevant to simulation tests:
///   #. k_factor_in : a conversion factor for input commodity or request.
///   #. k_factor_out : a conversion factor for output commodity or bid.
///   #. in_capacity : an initial capacity for input commodity.
///   #. out_capacity : an initial capacity for output commodity.
class KFacility : public cyclus::Facility {
 public:
  /// @brief Constructor for KFacility Class
  /// @param ctx the cyclus context for access to simulation-wide parameters
  KFacility(cyclus::Context* ctx);
  virtual ~KFacility();

  #pragma cyclus

  #pragma cyclus note {"doc": "A facility designed for integration tests " \
                              "that both provides and consumes commodities" \
                              ". It changes its request and offer amounts " \
                              "based on a power law with respect to time."}

  virtual std::string str();

  virtual void Tick();

  virtual void Tock();

  /// @brief Responds to each request for this source facility's commodity.
  /// If a given request is more than this facility's capacity, it will offer
  /// its capacity.
  virtual std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
      GetMatlBids(
          cyclus::CommodMap<cyclus::Material>::type& commod_requests);

  /// @brief respond to each trade with a material made from this facility's
  /// recipe
  ///
  /// @param trades all trades in which this trader is the supplier
  /// @param responses a container to populate with responses to each trade
  virtual void GetMatlTrades(
      const std::vector< cyclus::Trade<cyclus::Material> >& trades,
      std::vector<std::pair<cyclus::Trade<cyclus::Material>,
      cyclus::Material::Ptr> >& responses);

  /// @brief Request Materials of their given commodity. Note
  /// that it is assumed the facility operates on a single resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests();

  /// @brief Request Products of their given
  /// commodity. Note that it is assumed the facility operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
      GetProductRequests();

  /// @brief Place accepted trade Materials in their Inventory
  virtual void AcceptMatlTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
      cyclus::Material::Ptr> >& responses);

  /// @brief Place accepted trade Materials in their Inventory
  virtual void AcceptProductTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
      cyclus::Product::Ptr> >& responses);

  // --- KFacility Members ---
  /// @brief creates a material object to offer to a requester
  /// @param target the material target a request desires
  cyclus::Material::Ptr GetOffer(const cyclus::Material::Ptr target) const;

  /// @brief sets the output commodity name
  /// @param name the commodity name
  inline void commodity(std::string name) { out_commod = name; }

  /// @return the output commodity
  inline std::string commodity() const { return out_commod; }

  /// @brief sets the capacity of a material generated at any given time step
  /// @param capacity the production capacity
  inline void capacity(double capacity) {
    out_capacity = capacity;
    current_capacity = out_capacity;
  }

  /// @return the production capacity at any given time step
  inline double capacity() const { return out_capacity; }

  /// @brief sets the name of the recipe to be produced
  /// @param name the recipe name
  inline void recipe(std::string name) { recipe_name = name; }

  /// @return the name of the output recipe
  inline std::string recipe() const { return recipe_name; }

  /// @return the current timestep's capacity
  inline double CurrentCapacity() const { return current_capacity; }

  /// @brief determines the amount to request
  inline double RequestAmt() const { return in_capacity; }

  /// @ return the conversion factor input
  inline double KFactorIn() const { return k_factor_in; }

  /// @brief sets the conversion factor input
  /// @param new conversion factor
  inline void KFactorIn(double k_factor) { k_factor_in = k_factor; }

  /// @ return the conversion factor for output
  inline double KFactorOut() const { return k_factor_out; }

  /// @brief sets the conversion factor for output
  /// @param new conversion factor
  inline void KFactorOut(double k_factor) { k_factor_out = k_factor; }

 private:
  /// This facility has one output commodity and one input commodity
  #pragma cyclus var {"tooltip": "input commodity", \
                      "doc": "commodity that the k-facility consumes", \
                      "schematype": "token"}
  std::string in_commod;

  #pragma cyclus var {"tooltip": "output commodity", \
                      "doc": "commodity that the k-facility supplies", \
                      "schematype": "token"}
  std::string out_commod;

  /// Name of the recipe this facility uses.
  #pragma cyclus var {"shape": [50], "tooltip": "in-commodity recipe name", \
                      "doc": "recipe name for the k-facility's in-commodity", \
                      "schematype": "token"}
  std::string recipe_name;

  /// The capacity is defined in terms of the number of units of the
  /// recipe that can be provided each time step.  A very large number
  /// can be provided to represent infinte capacity.
  /// In and out commodity capacities are defined.
  #pragma cyclus var {"tooltip": "input commodity capacity", \
                      "doc": "number of commodity units that can be taken " \
                             "at each time step (infinite capacity can be " \
                             "represented by a very large number"}
  double in_capacity;

  #pragma cyclus var {"tooltip": "output commodity capacity", \
                      "doc": "number of commodity units that can be " \
                             "supplied at each time step (infinite " \
                             "capacity can be represented by a very large " \
                             "number"}
  double out_capacity;

  /// The output capacity at the current time step.
  #pragma cyclus var {"default": 0, "tooltip": "current output capacity", \
                      "doc": "number of output commodity units that can be " \
                             "supplied at the current time step (infinite " \
                             "capacity can be represented by a very large " \
                             "number"}
  double current_capacity;

  #pragma cyclus var {"default": 1e299, "tooltip": "k-facility maximum " \
                                                   "inventory size", \
                      "doc": "total maximum inventory size of the k-facility"}
  double max_inv_size;

  #pragma cyclus var {'capacity': 'max_inv_size'}
  cyclus::toolkit::ResourceBuff inventory;

  /// Conversion factors for input and output amounts.
  #pragma cyclus var {"tooltip": "input k-factor", \
                      "doc": "conversion factor that governs the behavior " \
                             "of the k-facility's input commodity capacity"}
  double k_factor_in;

  #pragma cyclus var {"tooltip": "output k-factor", \
                      "doc": "conversion factor that governs the behavior " \
                             "of the k-facility's output commodity capacity"}
  double k_factor_out;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_K_FACILITY_H_

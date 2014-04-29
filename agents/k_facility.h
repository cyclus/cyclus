#ifndef CYCLUS_AGENTS_K_FACILITY_H_
#define CYCLUS_AGENTS_K_FACILITY_H_

#include <set>
#include <string>
#include <vector>

#include "cyclus.h"

namespace cyclus {

class Context;

/**
   @class KFacility

   @section intro Introduction
   The KFacility is a source and consumer of commodities.
   The facility changes its request and offer amount according to a power law.

   Amount = Capacity(T=0) * ConversionFactor ^ Time

   This facility is intended to be used for Cyclus trivial and
   minimal cycle integration tests.

   @section params Parameters
   The parameters relevant to simulation tests:
      #. k_factor_in_ : a conversion factor for input commodity or request.
      #. k_factor_out_ : a conversion factor for output commodity or bid.
      #. in_capacity_ : an initial capacity for input commodity.
      #. out_capacity_ : an initial capacity for output commodity.
 */
class KFacility : public cyclus::Facility {
 public:
  /**
     @brief Constructor for KFacility Class
     @param ctx the cyclus context for access to simulation-wide parameters
   */
  KFacility(cyclus::Context* ctx);
  virtual ~KFacility();

  #pragma cyclus decl

  virtual std::string str();

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

  /// @brief Request Materials of their given commodity. Note
  /// that it is assumed the facility operates on a single resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests();

  /// @brief Request Products of their given
  /// commodity. Note that it is assumed the facility operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
      GetGenRsrcRequests();

  /// @brief Place accepted trade Materials in their Inventory
  virtual void AcceptMatlTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
      cyclus::Material::Ptr> >& responses);

  /// @brief Place accepted trade Materials in their Inventory
  virtual void AcceptGenRsrcTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
      cyclus::Product::Ptr> >& responses);

  /* --- KFacility Members --- */
  /**
     @brief creates a material object to offer to a requester
     @param target the material target a request desires
   */
  cyclus::Material::Ptr GetOffer(const cyclus::Material::Ptr target) const;

  /**
     @brief sets the output commodity name
     @param name the commodity name
   */
  inline void commodity(std::string name) { out_commod_ = name; }

  /// @return the output commodity
  inline std::string commodity() const { return out_commod_; }

  /**
     @brief sets the capacity of a material generated at any given time step
     @param capacity the production capacity
   */
  inline void capacity(double capacity) {
    out_capacity_ = capacity;
    current_capacity_ = out_capacity_;
  }

  /// @return the production capacity at any given time step
  inline double capacity() const { return out_capacity_; }

  /**
     @brief sets the name of the recipe to be produced
     @param name the recipe name
   */
  inline void recipe(std::string name) { recipe_name_ = name; }

  /// @return the name of the output recipe
  inline std::string recipe() const { return recipe_name_; }

  /// @return the current timestep's capacity
  inline double current_capacity() const { return current_capacity_; }

  /**
     @brief determines the amount to request
   */
  inline double RequestAmt() const { return in_capacity_; }

  /// @ return the conversion factor input
  inline double k_factor_in() const { return k_factor_in_; }

  /// @brief sets the conversion factor input
  /// @param new conversion factor
  inline void k_factor_in(double k_factor) { k_factor_in_ = k_factor; }

  /// @ return the conversion factor for output
  inline double k_factor_out() const { return k_factor_out_; }

  /// @brief sets the conversion factor for output
  /// @param new conversion factor
  inline void k_factor_out(double k_factor) { k_factor_out_ = k_factor; }

 private:
  /**
     This facility has one output commodity and one input commodity
   */
  #pragma cyclus var {}
  std::string in_commod_;

  #pragma cyclus var {}
  std::string out_commod_;

  /**
     Name of the recipe this facility uses.
   */
  #pragma cyclus var {}
  std::string recipe_name_;

  /**
     The capacity is defined in terms of the number of units of the
     recipe that can be provided each time step.  A very large number
     can be provided to represent infinte capacity.
     In and out commodity capacities are defined.
   */
  #pragma cyclus var {}
  double in_capacity_;

  #pragma cyclus var {}
  double out_capacity_;

  /**
     The output capacity at the current time step.
   */
  #pragma cyclus var {"default": 0}
  double current_capacity_;

  #pragma cyclus var {"default": 1e299}
  double max_inv_size_;

  #pragma cyclus var {'capacity': 'max_inv_size_'}
  cyclus::ResourceBuff inventory_;

  /**
     Conversion factors for input and output amounts.
   */
  #pragma cyclus var {}
  double k_factor_in_;

  #pragma cyclus var {}
  double k_factor_out_;
};

}  // namespace cyclus

#endif  // CYCLUS_AGENTS_K_FACILITY_H_

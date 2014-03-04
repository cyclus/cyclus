#ifndef CYCLUS_INTEGRATION_TESTS_MODELS_SINK_FACILITY_H_
#define CYCLUS_INTEGRATION_TESTS_MODELS_SINK_FACILITY_H_

#include <string>

#include "context.h"
#include "facility_model.h"
#include "generic_resource.h"
#include "logger.h"
#include "material.h"
#include "query_engine.h"
#include "request_portfolio.h"
#include "resource_buff.h"
#include "trade.h"

namespace cyclus {

/**
  @class SimpleSink
  This sink facility accepts specified amount of commodity.
  This sink facility is similar to SimpleSink provided in cycamore, but it
  has minimum implementation to run integration tests.
  Some parts of the code is directrly copied from cycamore SimpleSink.

  This FacilityModel is intended
  to be used in Cyclus integration tests as a basic sink facility.

  @section intro Introduction

  @section modelparams Model Parameters

  @section optionalparams Optional Parameters

  @section detailed Detailed Behavior
  */
class SimpleSink : public cyclus::FacilityModel  {
  /* --------------------
   * all FACILITYMODEL classes have these members
   * --------------------
   */
 public:
  /**
    Constructor for SimpleSink Class
    @param ctx the cyclus context for access to simulation-wide parameters
    */
  explicit SimpleSink(cyclus::Context* ctx);

  /**
    Distructor for SimpleSink
    */
  virtual ~SimpleSink();
  /**
    Schema for SimpleSink
    */
  virtual std::string schema();

  /**
    Initialize members related to derived module class

    @param qe a pointer to a QueryEngine object containing initialization data
    */
  virtual void InitFrom(cyclus::QueryEngine* qe);

  /**
    Initialize members for a cloned module.
    */
  virtual void InitFrom(SimpleSink* m);

  /**
    A verbose printer for the SimpleSink
    */
  virtual std::string str();

  /**
    Initializes a SimpleSink object by copying the members of another.
    */
  virtual cyclus::Model* Clone();

  /**
    The handleTick function specific to the SimpleSink.

    @param time the time of the tick
    */
  virtual void Tick(int time);

  /**
    The handleTick function specific to the SimpleSink.

    @param time the time of the tock
    */
  virtual void Tock(int time);

  /* ------------------- */

  /// @brief SinkFacilities request Materials of their given commodity. Note
  /// that it is assumed the SimpleSink operates on a single resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests();

  /// @brief SinkFacilities request GenericResources of their given
  /// commodity. Note that it is assumed the SimpleSink operates on a single
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
  /* --- */

  /* --------------------
   * _THIS_ FACILITYMODEL class has these members
   * --------------------
   */
  /**
     add a commodity to the set of input commodities
     @param name the commodity name
   */
  inline void AddCommodity(std::string name) {
    in_commods_.push_back(name);
  }

  /**
    determines the amount to request
    */
  inline double RequestAmt() const {
    return capacity_;
  }

  /* ------------------- */
 private:
  std::vector<std::string> in_commods_;
  std::string incommodity_;
  double capacity_;
  cyclus::ResourceBuff inventory_;
};

}  // namespace cyclus

#endif  // CYCLUS_INTEGRATION_TESTS_MODELS_SINK_FACILITY_H_

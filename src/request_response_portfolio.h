#ifndef CYCLUS_REQUEST_RESPSONSE_PORTFOLIO_H_
#define CYCLUS_REQUEST_RESPSONSE_PORTFOLIO_H_

#include "capacity_constraint.h"
#include "facility_model.h"
#include "request_response.h"

namespace cyclus {

/// @class ResponsePortfolio contains all the information corresponding to a
/// responder to resource requests. It is a light wrapper around the set
/// of responses and constraints for a given responder, guaranteeing a single
/// responder per portfolio.
template <class T>
class ResponsePortfolio {
 public:
  /// @brief default constructor
  ResponsePortfolio() : responder_(NULL), commodity_("") { };
  
  /// @return the model associated with the portfolio. if no responses have
  /// been added, the responder is NULL.
  const cyclus::FacilityModel* responder() {
    return responder_;
  };
    
  /// @return the commodity associated with the portfolio. if no responses have
  /// been added, the commodity is empty.
  std::string commodity() {
    return commodity_;
  };

  /// @brief add a response to the portfolio
  /// @param r the response to add
  /// @throws if a response is added from a different responder than the original
  void AddResponse(const cyclus::RequestResponse<T>& r) {
    VerifyResponder(r);
    VerifyCommodity(r);
    responses_.insert(r);
  };

  /// @brief add a capacity constraint associated with the portfolio
  /// @param c the constraint to add
  void AddConstraint(const cyclus::CapacityConstraint<T>& c) {
    constraints_.insert(c);
  };

  /// @return const access to the responses
  const std::set< cyclus::RequestResponse<T> >& responses() {
    return responses_;
  };
  
  /// @return the set of constraints over the responses
  const std::set< cyclus::CapacityConstraint<T> >& constraints() {
    return constraints_;
  };

 private:
  /// @brief if the responder has not been determined yet, it is set. otherwise
  /// VerifyResponder() verifies the the response is associated with the
  /// portfolio's responder
  /// @throws if a response is added from a different responder than the original
  void VerifyResponder(const cyclus::RequestResponse<T> r) {
    if (responder_ == NULL) {
      responder_ = r.responder;
    } else if (responder_ != r.responder) {
      std::string msg = "Can't insert a response from " + r.responder->name()
          + " into " + responder_->name() + "'s portfolio.";
      throw cyclus::KeyError(msg);
    }
  };

  /// @brief if the commodity has not been determined yet, it is set. otherwise
  /// VerifyCommodity() verifies the the commodity is associated with the
  /// portfolio's commodity
  /// @throws if a commodity is added that is a different commodity from the
  /// original
  void VerifyCommodity(const cyclus::RequestResponse<T> r) {
    if (commodity_ == "") {
      commodity_ = r.commodity;
    } else if (commodity_ != r.commodity) {
      std::string msg = "Commodity mismatch for a request response: "
                        + r.commodity() + " != " + commodity_ + ".";
      throw cyclus::KeyError(msg);
    }
  };
  
  std::string commodity_;
  cyclus::FacilityModel* responder_;
  std::set< cyclus::RequestResponse<T> > responses_;
  std::set< cyclus::CapacityConstraint<T> > constraints_;
};

} // namespace cyclus

#endif

#ifndef CYCLUS_SRC_TOOLKIT_MATL_BUY_POLICY_H_
#define CYCLUS_SRC_TOOLKIT_MATL_BUY_POLICY_H_

#include <string>

#include "composition.h"
#include "material.h"
#include "resource_buff.h"
#include "trader.h"

namespace cyclus {
namespace toolkit {

/// MatlBuyPolicy performs semi-automatic inventory management of a material
/// buffer by making requests and accepting materials in an attempt to fill the
/// buffer fully every time step according to an (s, S) inventory policy (see
/// [1]).
///
/// For simple behavior, policies virtually eliminate the need to write any code
/// for resource exchange. Just assign a few policies to work with a few buffers
/// and focus on writing the physics and other behvavior of your agent.  Typical
/// usage goes something like this:
///
/// @code
/// class YourAgent : public cyclus::Facility {
///  public:
///   ...
///
///   void EnterNotify() {
///     cyclus::Facility::EnterNotify(); // always do this first
///
///     policy_.Init(this, &inbuf_, "inbuf-label").Set(incommod, comp).Start();
///   }
///   ...
///
///  private:
///   MatlBuyPolicy policy_;
///   ResourceBuff inbuf_;
///    ...
/// }
/// @endcode
///
/// The policy needs to be initialized with its owning agent and the material
/// buffer that is is managing. It also needs to be activated by calling the
/// Start function for it to begin participation in resource exchange.  And
/// don't forget to add some commodities to request by calling Set.  All policy
/// configuration should usually occur in the agent's EnterNotify member
/// function.
///
/// [1] Zheng, Yu-Sheng. "A simple proof for optimality of (s, S) policies in
/// infinite-horizon inventory systems." Journal of Applied Probability
/// (1991): 802-810.
///
/// @warn When a policy's managing agent is deallocated, you MUST either
/// call the policy's Stop function or delete the policy. Otherwise SEGFAULT.
class MatlBuyPolicy : public Trader {
 public:
  /// Creates an uninitialized policy.  The Init function MUST be called before
  /// anything else is done with the policy.
  MatlBuyPolicy();

  virtual ~MatlBuyPolicy();

  /// Configures the policy to keep buf filled to a certain fraction of its
  /// capacity every time step.
  /// @param manager the agent
  /// @param buf the resource buffer
  /// @param name a unique name identifying this policy
  /// @param quantize If quantize is greater than zero, the policy will make
  /// exclusive, integral quantize kg requests.  Otherwise, single requests will
  /// be sent to fill the buffer's empty space.
  /// @param fill_to_frac the fraction of inventory to order when placing an
  /// order. This is equivalent to the S in an (s, S) inventory policy.
  /// @param req_when_frac place an request when the buf's quantity is less than
  /// its capacity * order_when_frac. This is equivalent to the s in an (s, S)
  /// inventory policy.
  MatlBuyPolicy& Init(Agent* manager, ResourceBuff* buf, std::string name,
                      double quantize = -1,
                      double fill_to = 1., double req_when_under = 1.);
                      
  /// Instructs the policy to fill its buffer with requests on the given
  /// commodity of composition c and the given preference.  This must be called
  /// at least once or the policy will do nothing.  The policy can request on an
  /// arbitrary number of commodities by calling Set multiple times.  Re-calling
  /// Set to modify the composition or preference of a commodity that has been
  /// set previously is allowed.
  /// @param commod the commodity name
  /// @param c the composition to request for the given commodity
  /// @param pref the preference value for the commodity
  MatlBuyPolicy& Set(std::string commod, Composition::Ptr c, double pref=1.0);

  /// Registers this policy as a trader in the current simulation.  This
  /// function must be called for the policy to begin participating in resource
  /// exchange. Init MUST be called prior to calling this function.  Start is
  /// idempotent.
  void Start();

  /// Unregisters this policy as a trader in the current simulation. Stop is
  /// idempotent.
  void Stop();

  /// the total amount requested
  inline double TotalQty() {
    return fill_to_ * buf_->capacity() - buf_->quantity();
  }
  
  /// the amount requested per each request
  inline double ReqQty() {
    if (quantize_ > 0)
      return quantize_;
    return TotalQty();
  }
  
  /// the number of requests made per each commodity
  inline int NReq() {
    return quantize_ > 0 ? static_cast<int>(TotalQty() / quantize_) : 1;
  }
  
  /// Returns corresponding commodities from which each material object
  /// was received for the current time step. The data returned by this function
  /// are ONLY valid during the Tock phase of a time step.
  std::map<Material::Ptr, std::string> Commods();

  virtual std::set<RequestPortfolio<Material>::Ptr> GetMatlRequests();

  virtual void AcceptMatlTrades(
      const std::vector<std::pair<Trade<Material>, Material::Ptr> >& resps);

 private:
  struct CommodDetail {
    Composition::Ptr comp;
    double pref;
  };
  Agent* manager_;
  ResourceBuff* buf_;
  std::string name_;
  double fill_to_, req_when_under_, quantize_;
  std::map<Material::Ptr, std::string> rsrc_commod_;
  std::map<std::string, CommodDetail> commods_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif

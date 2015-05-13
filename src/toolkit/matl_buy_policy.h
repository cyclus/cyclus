#ifndef CYCLUS_SRC_TOOLKIT_MATL_BUY_POLICY_H_
#define CYCLUS_SRC_TOOLKIT_MATL_BUY_POLICY_H_

#include <string>

#include "composition.h"
#include "material.h"
#include "res_buf.h"
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
///   ResBuf<Material> inbuf_;
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
  /// @param throughput a constraining value for total transaction quantities in
  /// a single time step
  /// @param fill_to the amount or fraction of inventory to order when placing
  /// an order. This is equivalent to the S in an (s, S) inventory policy.
  /// @param req_when_under place an request when the buf's quantity is less
  /// than its capacity * fill_to (as a fraction). This is equivalent to the s
  /// in an (s, S) inventory policy.
  /// @param quantize If quantize is greater than zero, the policy will make
  /// exclusive, integral quantize kg requests.  Otherwise, single requests will
  /// be sent to fill the buffer's empty space.
  /// @warning, (s, S) policy values are ambiguous for buffers with a capacity
  /// in (0, 1]. However that is a rare case.
  /// @{
  MatlBuyPolicy& Init(Agent* manager, ResBuf<Material>* buf, std::string name);
  MatlBuyPolicy& Init(Agent* manager, ResBuf<Material>* buf, std::string name,
                      double throughput);
  MatlBuyPolicy& Init(Agent* manager, ResBuf<Material>* buf, std::string name,
                      double fill_to, double req_when_under);
  MatlBuyPolicy& Init(Agent* manager, ResBuf<Material>* buf, std::string name,
                      double throughput, double fill_to,
                      double req_when_under, double quantize);
  /// @}
    
  /// Instructs the policy to fill its buffer with requests on the given
  /// commodity of composition c and the given preference.  This must be called
  /// at least once or the policy will do nothing.  The policy can request on an
  /// arbitrary number of commodities by calling Set multiple times.  Re-calling
  /// Set to modify the composition or preference of a commodity that has been
  /// set previously is allowed.
  /// @param commod the commodity name
  /// @param c the composition to request for the given commodity
  /// @param pref the preference value for the commodity
  /// @{
  MatlBuyPolicy& Set(std::string commod);
  MatlBuyPolicy& Set(std::string commod, Composition::Ptr c);
  MatlBuyPolicy& Set(std::string commod, Composition::Ptr c, double pref);
  /// @}

  /// Registers this policy as a trader in the current simulation.  This
  /// function must be called for the policy to begin participating in resource
  /// exchange. Init MUST be called prior to calling this function.  Start is
  /// idempotent.
  void Start();

  /// Unregisters this policy as a trader in the current simulation. This
  /// function need only be called if a policy is to be stopped *during* a
  /// simulation (it is not required to be called explicitly at the end). Stop
  /// is idempotent.
  void Stop();

  /// the total amount requested
  inline double TotalQty() const {
    return std::min(throughput_,
                    fill_to_ * buf_->capacity() - buf_->quantity());
  }

  /// whether trades will be denoted as exclusive or not
  inline bool Excl() const { return quantize_ > 0; }
  
  /// the amount requested per each request
  inline double ReqQty() const {
    return Excl() ? quantize_ : TotalQty();
  }
  
  /// the number of requests made per each commodity
  inline int NReq() const {
    return Excl() ? static_cast<int>(TotalQty() / quantize_) : 1;
  }
  
  /// Returns corresponding commodities from which each material object
  /// was received for the current time step. The data returned by this function
  /// are ONLY valid during the Tock phase of a time step.
  inline const std::map<Material::Ptr, std::string>& rsrc_commods() {
      return rsrc_commods_;
  };

  /// Trader Methods
  /// @{
  virtual std::set<RequestPortfolio<Material>::Ptr> GetMatlRequests();
  virtual void AcceptMatlTrades(
      const std::vector<std::pair<Trade<Material>, Material::Ptr> >& resps);
  /// }@

 private:
  struct CommodDetail {
    Composition::Ptr comp;
    double pref;
  };

  /// requires buf_ already set
  void set_fill_to(double x); 
  /// requires buf_ already set
  void set_req_when_under(double x); 
  void set_quantize(double x); 
  void set_throughput(double x); 
  
  ResBuf<Material>* buf_;
  std::string name_;
  double fill_to_, req_when_under_, quantize_, throughput_;
  std::map<Material::Ptr, std::string> rsrc_commods_;
  std::map<std::string, CommodDetail> commod_details_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif

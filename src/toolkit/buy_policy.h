#ifndef CYCLUS_SRC_TOOLKIT_BUY_POLICY_H_
#define CYCLUS_SRC_TOOLKIT_BUY_POLICY_H_

#include <string>

#include "composition.h"
#include "material.h"
#include "resource_buff.h"
#include "trader.h"

namespace cyclus {
namespace toolkit {

/// BuyPolicy performs semi-automatic inventory management of a material
/// buffer by making requests and accepting materials in an attempt to fill the
/// buffer fully every time step.
/// For simple behavior, policies virtually eliminate the need to write any code
/// for resource exchange. Just assign a few policies to work with a few
/// buffers and focus on writing the physics and other behvavior of your agent.
/// Typical usage goes something like this:
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
///   BuyPolicy policy_;
///   ResourceBuff inbuf_;
///    ...
/// }
/// @endcode
///
/// The policy needs to be initialized with its owning agent and the material
/// buffer that is is managing. It also needs to be activated by calling the
/// Start function for it to begin participation in resource exchange.
/// And don't forget to add some commodities to request by calling Set.  All
/// policy configuration should usually occur in the agent's EnterNotify
/// member function.
///
/// @warn When a policy's managing agent is deallocated, you MUST either
/// call the policy's Stop function or delete the policy. Otherwise SEGFAULT.
class BuyPolicy : public Trader {
 public:
  /// Creates an uninitialized policy.  The Init function MUST be called before
  /// anything else is done with the policy.
  BuyPolicy();

  virtual ~BuyPolicy();

  /// Configures the policy to keep buf full every time step.  If
  /// quantize is greater than zero, the policy will make exclusive, integral
  /// quantize kg requests.  Otherwise, single requests will be sent to
  /// fill the buffer's empty space.
  BuyPolicy& Init(Agent* manager, ResourceBuff* buf, std::string name,
                  double quantize = -1);

  /// Instructs the policy to fill its buffer with requests on the given
  /// commodity of composition c and the given preference.  This must be called
  /// at least once or the policy will do nothing.  The policy can
  /// request on an arbitrary number of commodities by calling Set multiple
  /// times.  Re-calling Set to modify the composition
  /// or preference of a commodity that has been set previously is allowed.
  BuyPolicy& Set(std::string commod, Composition::Ptr c, double pref = 0.0);

  /// Registers this policy as a trader in the current simulation.  This
  /// function must be called for the policy to begin participating in resource
  /// exchange. Init MUST be called prior to calling this function.  Start is
  /// idempotent.
  void Start();

  /// Unregisters this policy as a trader in the current simulation. Stop is
  /// idempotent.
  void Stop();

  /// Returns corresponding commodities from which each material object
  /// was received for the current time step. The data returned by this function
  /// are ONLY valid during the Tock phase of a time step.
  std::map<Material::Ptr, std::string> Commods();

  virtual std::set<RequestPortfolio<Material>::Ptr> GetMatlRequests();

  virtual void AcceptMatlTrades(
      const std::vector<std::pair<Trade<Material>, Material::Ptr> >& resps);

  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs);

 private:
  struct CommodDetail {
    Composition::Ptr comp;
    double pref;
  };

  double quantize_;

  std::string name_;

  std::map<Material::Ptr, std::string> rsrc_commod_;

  ResourceBuff* buf_;

  std::map<std::string, CommodDetail> commods_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif

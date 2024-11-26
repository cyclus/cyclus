#ifndef CYCLUS_SRC_TOOLKIT_MATL_BUY_POLICY_H_
#define CYCLUS_SRC_TOOLKIT_MATL_BUY_POLICY_H_

#include <string>
#include <boost/shared_ptr.hpp>

#include "composition.h"
#include "material.h"
#include "res_buf.h"
#include "total_inv_tracker.h"
#include "trader.h"
#include "random_number_generator.h"

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
  /// @param inv_policy the inventory policy to use. Options are "sS", "RQ". 
  /// Each inventory policy options has two additional required parameters. An 
  /// (s,S) inventory policy orders material only when the buffer is below the 
  /// minimum value, s, and orders only enough to bring the buffer to the 
  /// maximum value, S. An (s,S) policy allows partial orders. Set s as req_at 
  /// and S as fill. An (R,Q) inventory policy requests material when the 
  /// buffer is below the minimum value, R, and places an exclusive request for 
  /// size Q. Set R as req_at and Q as fill. 
  /// @param req_at, the inventory minimum (s, and R) for (s,S) and (R,Q),. If 
  /// the buffer has less than or equal to value, new material will be 
  /// requested based on the policy in place. 
  /// @param fill_behav, the quantity govering the fill strategy for inventory 
  /// policies. For (s,S), this is the maximum value, and material will be 
  /// ordered up to this amount. For (R,Q), this is the quantity of material 
  /// that will be ordered (exclusive).
  /// @param cumulative_cap the cumulative capacity of material to be received 
  /// in one active cycle. A cumulative cap inventory policy allows for a 
  /// cumulative capacity of material to be received in one active cycle. Once 
  /// the cumulative capacity is recieved, the agent enters a dormant period. 
  /// Also requires dormant distributions using dormant_dist.
  /// @param quantize If quantize is greater than zero, the policy will make
  /// exclusive, integral quantize kg requests.  Otherwise, single requests will
  /// be sent to fill the buffer's empty space.
  /// @warning, (s, S) policy values are ambiguous for buffers with a capacity
  /// in (0, 1]. However that is a rare case.
  /// The following features, if used, will set the policy to so that agents
  /// cycle through "on" phases, where the buf will attemp to be filled, and
  /// "off" or dormant phases, where no requests will be made regardless of
  /// capacity.  The "on" and "off" phases are sampled and rounded to the
  /// nearest integer number of time steps from a truncated normal
  /// distribution from a mean, standard deviation, min, and max value.
  /// Note that the (s, S) policy is not currently compatible with active and
  /// Active and dormant buying perionds 
  /// Active parameters:
  /// @param active_dist an IntDistribution object that will be used to sample
  /// the number of time steps in the "on" phase.  If not set, a fixed
  /// distribution with a value of 1 will be used.
  /// @param dormant_dist an IntDistribution object that will be used to sample
  /// the number of time steps in the "off" phase.  If not set, a fixed
  /// distribution with a value of 0 will be used (no dormant, always on)
  /// @param size_dist a DoubleDistribution object that will be used to sample
  /// the size of the request as a fraction of the available capacity at the 
  /// current time step.  If not set, a fixed distribution with a value of 
  /// 1.0 will be used.
  /// Note that active and dormant periods are note currently compatible with
  /// (s, S) inventory management
  /// @{
  MatlBuyPolicy& Init(Agent* manager, ResBuf<Material>* buf, std::string name,
                      TotalInvTracker* buf_tracker);
  MatlBuyPolicy& Init(Agent* manager, ResBuf<Material>* buf, std::string name,
                      TotalInvTracker* buf_tracker, double throughput,
                      IntDistribution::Ptr active_dist = NULL, 
                      IntDistribution::Ptr dormant_dist = NULL,
                      DoubleDistribution::Ptr size_dist = NULL);
  MatlBuyPolicy& Init(Agent* manager, ResBuf<Material>* buf, std::string name,
                      TotalInvTracker* buf_tracker, double throughput,
                      double quantize);                    
  MatlBuyPolicy& Init(Agent* manager, ResBuf<Material>* buf, std::string name,
                      TotalInvTracker* buf_tracker,
                      double throughput, std::string inv_policy,
                      double fill_behav, double req_at);
  MatlBuyPolicy& Init(Agent* manager, ResBuf<Material>* buf, std::string name,
                      TotalInvTracker* buf_tracker, std::string inv_policy,
                      double fill_behav, double req_at);
  MatlBuyPolicy& Init(Agent* manager, ResBuf<Material>* buf, std::string name,
                      TotalInvTracker* buf_tracker, double throughput,
                      double cumulative_cap,
                      IntDistribution::Ptr);
  /// @}

  /// Reset a material buy policy parameters that govern its behavior 
  /// to the default state.  Preserve the Trader that manages the policy.
  /// The primary use case of this method is when there is a desire to
  /// change the behavior of a policy. This reset can be called prior
  /// to calling a new `Init()` to establish the new behavior.
  MatlBuyPolicy& ResetBehavior();

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

  /// Instructs the policy to stop requesting a speific commodity
  MatlBuyPolicy& Unset(std::string commod);

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

  /// the total amount available to request
  inline double TotalAvailable() const {
    return std::min({throughput_,
                    fill_to_ - buf_->quantity(),
                    buf_->space(),
                    buf_tracker_->space()});
  }

  /// whether a request can be made
  inline bool MakeReq() const { return buf_tracker_->quantity() <= req_at_; }

  /// whether trades will be denoted as exclusive or not
  inline bool Excl() const { return quantize_ > 0; }


  /// the amount requested per each request
  inline double ReqQty(double amt) const {
    return Excl() ? quantize_ : amt;
  }

  /// the number of requests made per each commodity
  inline int NReq(double amt) const {
    return Excl() ? static_cast<int>(amt / quantize_) : 1;
  }

  /// Returns corresponding commodities from which each material object
  /// was received for the current time step. The data returned by this function
  /// are ONLY valid during the Tock phase of a time step.
  inline const std::map<Material::Ptr, std::string>& rsrc_commods() {
      return rsrc_commods_;
  };

  inline bool no_cycle_end_time() {
    return (next_dormant_end_ < 0 || next_active_end_ < 0);
  }

  inline bool dormant(int time) { 
    return (time >= next_active_end_ && time < next_dormant_end_); 
  }

  inline bool use_cumulative_capacity() { return cumulative_cap_ > 0; }

  /// Trader Methods
  /// @{
  virtual std::set<RequestPortfolio<Material>::Ptr> GetMatlRequests();
  virtual void AcceptMatlTrades(
      const std::vector<std::pair<Trade<Material>, Material::Ptr> >& resps);
  /// }@

  void SetNextActiveTime();
  void SetNextDormantTime();
  double SampleRequestSize();
  void CheckActiveDormantCumulativeTimes();
  void RecordActiveDormantTime(int time, std::string type, int length);

 private:
  struct CommodDetail {
    Composition::Ptr comp;
    double pref;
  };

  void set_manager(Agent* m);
  void set_total_inv_tracker(TotalInvTracker* t = NULL);
  void set_inv_policy(std::string p, double x, 
                      double y = std::numeric_limits<double>::max());
  /// requires buf_ already set
  void set_fill_to(double x);
  /// requires buf_ already set
  void set_req_at(double x);
  void set_cumulative_cap(double x);
  void set_quantize(double x);
  void set_throughput(double x);
  void init_active_dormant();

  ResBuf<Material>* buf_;
  TotalInvTracker* buf_tracker_;
  std::string name_, inv_policy;
  double fill_to_, req_at_, quantize_, throughput_, cumulative_cap_,
    cycle_total_inv_;

  int next_active_end_= 0;
  int next_dormant_end_= 0;

  IntDistribution::Ptr active_dist_;
  IntDistribution::Ptr dormant_dist_;
  DoubleDistribution::Ptr size_dist_;

  std::map<Material::Ptr, std::string> rsrc_commods_;
  std::map<std::string, CommodDetail> commod_details_;
};
  
}  // namespace toolkit
}  // namespace cyclus

#endif

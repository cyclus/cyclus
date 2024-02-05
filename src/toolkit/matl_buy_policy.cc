#include "matl_buy_policy.h"

#include <sstream>

#include "error.h"

#define LG(X) LOG(LEV_##X, "buypol")
#define LGH(X)                                                    \
  LOG(LEV_##X, "buypol") << "policy " << name_ << " (agent "      \
                         << Trader::manager()->prototype() << "-" \
                         << Trader::manager()->id() << "): "

namespace cyclus {
namespace toolkit {

MatlBuyPolicy::MatlBuyPolicy() :
    Trader(NULL),
    name_(""),
    throughput_(std::numeric_limits<double>::max()),
    quantize_(-1),
    fill_to_(std::numeric_limits<double>::max()),
    req_at_(std::numeric_limits<double>::max()),
    cumulative_cap_(-1),
    cycle_total_inv_(0),
    active_dist_(NULL),
    dormant_dist_(NULL),
    size_dist_(NULL){
  Warn<EXPERIMENTAL_WARNING>(
      "MatlBuyPolicy is experimental and its API may be subject to change");
}

MatlBuyPolicy::~MatlBuyPolicy() {
  if (manager() != NULL)
    manager()->context()->UnregisterTrader(this);
}

void MatlBuyPolicy::set_manager(Agent* m) {
  if (m != NULL) {
    Trader::manager_ = m;
  }
  else {
    std::stringstream ss;
    ss << "No manager set on Buy Policy " << name_;
    throw ValueError(ss.str());
  }
}

void MatlBuyPolicy::set_total_inv_tracker(TotalInvTracker* t) {
  if (t == NULL){
    std::vector<ResBuf<Material>*> bufs = {buf_};
    buf_tracker_->Init(bufs, buf_->capacity());
  }
  else {
    buf_tracker_ = t;
  }
}

void MatlBuyPolicy::set_inv_policy(std::string inv_policy, double fill, double req_at) {
  set_req_at(req_at);
  std::transform(inv_policy.begin(), inv_policy.end(), inv_policy.begin(), ::tolower);
  if ((inv_policy == "ss")) {
    set_fill_to(fill);
  }
  else if ((inv_policy == "rq") || (inv_policy == "qr")) {
    set_quantize(fill);
    // maximum amount that an RQ policy could achieve is req_at + fill
    set_fill_to(req_at + fill);
  }
  else {
    throw ValueError("Invalid inventory policy");
  }
}

void MatlBuyPolicy::set_fill_to(double x) {
  assert(x > 0);
  fill_to_ = x;
}

void MatlBuyPolicy::set_req_at(double x) {
  assert(x >= 0);
  req_at_ = x;
}

void MatlBuyPolicy::set_cumulative_cap(double x) {
  assert(x > 0);
  cumulative_cap_ = x;
}

void MatlBuyPolicy::set_quantize(double x) {
  assert(x != 0);
  quantize_ = x;
}

void MatlBuyPolicy::set_throughput(double x) {
  assert(x >= 0);
  throughput_ = x;
}

void MatlBuyPolicy::init_active_dormant() {
  if (active_dist_ == NULL) {
    active_dist_ = boost::shared_ptr<FixedIntDist>(new FixedIntDist(1));
  }
  if (dormant_dist_ == NULL) {
    dormant_dist_ = boost::shared_ptr<FixedIntDist>(new FixedIntDist(-1));
  }
  if (size_dist_ == NULL) {
    size_dist_ = boost::shared_ptr<FixedDoubleDist>(new FixedDoubleDist(1.0));
  }

  if (size_dist_->max() > 1) {
    throw ValueError("Size distribution cannot have a max greater than 1.");
  }
  
  SetNextActiveTime();
  LGH(INFO4) << "first active time end: " << next_active_end_ << std::endl;
 
  if (dormant_dist_->sample() < 0) {
    next_dormant_end_ = -1;
    LGH(INFO4) << "dormant length -1, always active" << std::endl;
    }
  else if (use_cumulative_capacity()) {
    next_dormant_end_ = -1;
    LGH(INFO4) << "dormant length set at -1 for first active period of cumulative capacity cycle" << std::endl;
    }
  else {
    SetNextDormantTime();
    LGH(INFO4) << "first dormant time end: " << next_dormant_end_ << std::endl;
  }
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                   std::string name, TotalInvTracker* buf_tracker) {
  set_manager(manager);
  buf_ = buf;
  name_ = name;
  set_total_inv_tracker(buf_tracker);
  init_active_dormant();
  return *this;
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                   std::string name, TotalInvTracker* buf_tracker,
                                   double throughput, boost::shared_ptr<IntDistribution> active_dist,
                                   boost::shared_ptr<IntDistribution> dormant_dist,
                                   boost::shared_ptr<DoubleDistribution> size_dist) {
  set_manager(manager);
  buf_ = buf;
  name_ = name;
  set_total_inv_tracker(buf_tracker);
  set_throughput(throughput);
  active_dist_ = active_dist;
  dormant_dist_ = dormant_dist;
  size_dist_ = size_dist;
  init_active_dormant();
  return *this;
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                   std::string name, TotalInvTracker* buf_tracker,
                                   double throughput, double quantize) {
  set_manager(manager);
  buf_ = buf;
  name_ = name;
  set_total_inv_tracker(buf_tracker);
  set_throughput(throughput);
  set_quantize(quantize);
  init_active_dormant();
  return *this;
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                   std::string name, 
                                   TotalInvTracker* buf_tracker,
                                   double throughput,
                                   std::string inv_policy,
                                   double fill_behav, double req_at) {
  set_manager(manager);
  buf_ = buf;
  name_ = name;
  set_total_inv_tracker(buf_tracker);
  set_inv_policy(inv_policy, fill_behav, req_at);
  set_throughput(throughput);
  init_active_dormant();
  return *this;
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                   std::string name, 
                                   TotalInvTracker* buf_tracker,
                                   std::string inv_policy,
                                   double fill_behav, double req_at) {
  set_manager(manager);
  buf_ = buf;
  name_ = name;
  set_total_inv_tracker(buf_tracker);
  set_inv_policy(inv_policy, fill_behav, req_at);
  init_active_dormant();
  return *this;
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf, 
                                   std::string name,
                                   TotalInvTracker* buf_tracker,
                                   double throughput, double cumulative_cap,
                                   boost::shared_ptr<IntDistribution> dormant_dist) {
  set_manager(manager);
  buf_ = buf;
  name_ = name;
  set_total_inv_tracker(buf_tracker);
  set_throughput(throughput);
  set_cumulative_cap(cumulative_cap);
  dormant_dist_ = dormant_dist;
  init_active_dormant();
  return *this;
}

MatlBuyPolicy& MatlBuyPolicy::Set(std::string commod) {
  CompMap c;
  c[10010000] = 1e-100;
  return Set(commod, Composition::CreateFromMass(c), 1.0);
}

MatlBuyPolicy& MatlBuyPolicy::Set(std::string commod, Composition::Ptr c) {
  return Set(commod, c, 1.0);
}

MatlBuyPolicy& MatlBuyPolicy::Set(std::string commod, Composition::Ptr c,
                                  double pref) {
  CommodDetail d;
  d.comp = c;
  d.pref = pref;
  commod_details_[commod] = d;
  return *this;
}

void MatlBuyPolicy::Start() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Buy Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->RegisterTrader(this);
}

void MatlBuyPolicy::Stop() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Buy Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->UnregisterTrader(this);
}

std::set<RequestPortfolio<Material>::Ptr> MatlBuyPolicy::GetMatlRequests() {
  rsrc_commods_.clear();
  std::set<RequestPortfolio<Material>::Ptr> ports;

  double amt;

  int current_time_ = manager()->context()->time();

  // Three step process to determine size of amount and reset any necessary
  // cycle times

  // Step 1: determine if inventory policy allows for a request to be made
  // Handing for (s,S)/(R,Q) inventory policies
  if (!MakeReq()) {
    return ports;
  }
  // Handling for cumulative capacity inventory policy
  // buy while not in dormant, don't buy while dormant. Make sure the request
  // is less than or equal to the space remaining in the cycle capacity
  // (cumulative cap minus current cycle inventory)
  if (use_cumulative_capacity()) {
    if (no_cycle_end_time() || current_time_ == next_dormant_end_){
      amt = std::min((TotalAvailable() * SampleRequestSize()),
                     (cumulative_cap_ - cycle_total_inv_));
    }
    else if (current_time_ < next_dormant_end_) {
      amt = 0;
      LGH(INFO3) << "in dormant period, no request" << std::endl;
      }
  }
  // Step 2: determine if active/dormant cycle times allow for a request
  // if no cycles, or if in the middle of active period, or if dormant period
  // just ended, then request
  else if (no_cycle_end_time() || (current_time_ < next_active_end_) || 
           (current_time_ == next_dormant_end_)) {
    amt = TotalAvailable() * SampleRequestSize();
  }
  // if in the middle of dormant period, then don't request
  else if (current_time_ < next_dormant_end_) {
    amt = 0;
    LGH(INFO3) << "in dormant period, no request" << std::endl;
  }

  // Step 3: Finally, determine if active/dormant cycle times need to be reset. 
  // If reaching the end of a cumulative cap cycle, set next_dormant_end_ = -1,
  // otherwise sample for next dormant.
  if (current_time_ == next_dormant_end_) {
    SetNextActiveTime();
    if (use_cumulative_capacity()) { next_dormant_end_ = -1; }
    else { 
      SetNextDormantTime();
      LGH(INFO4) << "end of dormant period, next active time end: " << next_active_end_ << ", and next dormant time end: " << next_dormant_end_ << std::endl;
      }
  }

  if (amt < eps())
    return ports;

  bool excl = Excl();
  double req_amt = ReqQty(amt);
  int n_req = NReq(amt);
  LGH(INFO3) << "requesting " << amt << " kg via " << n_req << " request(s)"  << std::endl;

  // one portfolio for each request
  for (int i = 0; i != n_req; i++) {
    RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
    std::vector<Request<Material>*> mreqs;
    std::map<std::string, CommodDetail>::iterator it;
    for (it = commod_details_.begin(); it != commod_details_.end(); ++it) {
      std::string commod = it->first;
      CommodDetail d = it->second;
      LG(INFO3) << "  - one " << amt << " kg request of " << commod;
      Material::Ptr m = Material::CreateUntracked(req_amt, d.comp);
      Request<Material>* r = port->AddRequest(m, this, commod, d.pref, excl);
      mreqs.push_back(r);
    }
    port->AddMutualReqs(mreqs);
    ports.insert(port);
  }

  return ports;
}

void MatlBuyPolicy::AcceptMatlTrades(
    const std::vector<std::pair<Trade<Material>, Material::Ptr> >& resps) {
  std::vector<std::pair<Trade<Material>, Material::Ptr> >::const_iterator it;
  rsrc_commods_.clear();
  for (it = resps.begin(); it != resps.end(); ++it) {
    rsrc_commods_[it->second] = it->first.request->commodity();
    LGH(INFO3) << "got " << it->second->quantity() << " kg of "
               << it->first.request->commodity()  << std::endl;
    buf_->Push(it->second);
    // cumulative capacity handling
    if (use_cumulative_capacity()) {
      cycle_total_inv_ += it->second->quantity();
    }
  }
  // check if cumulative cap has been reached. If yes, then sample for dormant
  // length and reset cycle_total_inv
  if (use_cumulative_capacity() && (
    (cumulative_cap_ - cycle_total_inv_) < eps())) {
      SetNextDormantTime();
      LGH(INFO3) << "cycle cumulative inventory has been reached. Dormant period will end at " << next_dormant_end_ << std::endl;
      cycle_total_inv_ = 0;
  }
}

void MatlBuyPolicy::SetNextActiveTime() {
  next_active_end_ = active_dist_->sample() + manager()->context()->time();
  return;
};

void MatlBuyPolicy::SetNextDormantTime() {
  if (use_cumulative_capacity()) {
    // need the +1 when not using next_active_end_ 
    next_dormant_end_ = (
      dormant_dist_->sample() + manager()->context()->time() + 1);
  }
  else if (next_dormant_end_ < 0) {}
  else {
    next_dormant_end_ = dormant_dist_->sample() + 
                        std::max(next_active_end_, 1);
  }
  return;
}

double MatlBuyPolicy::SampleRequestSize() {
  return size_dist_->sample();
}


}  // namespace toolkit
}  // namespace cyclus

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
    fill_to_(1),
    req_when_under_(1){
  Warn<EXPERIMENTAL_WARNING>(
      "MatlBuyPolicy is experimental and its API may be subject to change");
}

MatlBuyPolicy::~MatlBuyPolicy() {
  if (manager() != NULL)
    manager()->context()->UnregisterTrader(this);
}

void MatlBuyPolicy::set_fill_to(double x) {
  if (x > 1)
    x /= buf_->capacity();
  assert(x > 0 && x <= 1.);
  fill_to_ = x;
}

void MatlBuyPolicy::set_req_when_under(double x) {
  if (x > 1)
    x /= buf_->capacity();
  assert(x > 0 && x <= 1.);
  req_when_under_ = x;
}

void MatlBuyPolicy::set_quantize(double x) {
  assert(x != 0);
  quantize_ = x;
}

void MatlBuyPolicy::set_throughput(double x) {
  assert(x >= 0);
  throughput_ = x;
}

void MatlBuyPolicy::set_default_distributions() {
  active_dist_ = new FixedIntDist(1);
  dormant_dist_ = new FixedIntDist(-1);
  size_dist_ = new FixedDoubleDist(1.0);
  init_active_dormant();
}

void MatlBuyPolicy::init_active_dormant() {
  next_active_end_ = active_dist_->sample();
  int dormant_len = dormant_dist_->sample();
  if (dormant_len < 0) {
    next_dormant_end_ = -1;
  }
  else {
    next_dormant_end_ = dormant_len + next_active_end_;
  }
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                   std::string name) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_default_distributions();
  return *this;
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                   std::string name, double throughput, IntDistribution* active_dist,
                                   IntDistribution* dormant_dist,
                                   DoubleDistribution* size_dist) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_throughput(throughput);
  active_dist_ = active_dist;
  dormant_dist_ = dormant_dist;
  size_dist_ = size_dist;
  init_active_dormant();
  return *this;
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                   std::string name,
                                   double fill_to, double req_when_under) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_fill_to(fill_to);
  set_req_when_under(req_when_under);
  set_default_distributions();
  return *this;
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                   std::string name, double throughput,
                                   double fill_to, double req_when_under,
                                   double quantize) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_fill_to(fill_to);
  set_req_when_under(req_when_under);
  set_quantize(quantize);
  set_throughput(throughput);
  set_default_distributions();
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
  bool make_req = buf_->quantity() < req_when_under_ * buf_->capacity();
  double amt;

  int current_time_ = manager()->context()->time();

  if (current_time_ < next_active_end_ || next_dormant_end_ < 0) {
    // currently in the middle of active buying period
    SetRequestSize();
    amt = TotalQty();
  }
  else if (current_time_ < next_dormant_end_) {
    // currently in the middle of dormant period
    amt = 0;
    LGH(INFO3) << "in dormant period, no request" << std::endl;
  }
  else if (current_time_ == next_active_end_) {
    // finished active. starting dormancy and sample/set length of dormant period
    amt = 0;
    SetNextDormantTime();
  }
  // the following is an if rather than if-else statement because if dormant
  // length is zero, buy policy should return to active immediately
  if (current_time_ == next_dormant_end_) {
    // finished dormant. starting buying and sample/set length of active period
    SetRequestSize();
    amt = TotalQty();
    SetNextActiveTime();
  }

  if (!make_req || amt < eps())
    return ports;

  bool excl = Excl();
  double req_amt = Excl() ? quantize_ : amt;
  int n_req = Excl() ? static_cast<int>(amt / quantize_) : 1;
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
  }
}

void MatlBuyPolicy::SetNextActiveTime() {
  next_active_end_ = active_dist_->sample() + manager()->context()->time();
  return;
};

void MatlBuyPolicy::SetNextDormantTime() {
  if (next_dormant_end_ < 0) {}
  else {
    next_dormant_end_ = dormant_dist_->sample() + manager()->context()->time();
  }
  return;
}

void MatlBuyPolicy::SetRequestSize() {
  sample_fraction_ = size_dist_->sample();
  if (sample_fraction_ > 1) {
    sample_fraction_ = sample_fraction_ / size_dist_->max();
  }
  return;
}


}  // namespace toolkit
}  // namespace cyclus

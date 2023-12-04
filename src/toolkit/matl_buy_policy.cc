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
    req_when_under_(1),
    active_type_("Fixed"),
    dormant_type_("Fixed"),
    active_mean_(1),
    dormant_mean_(0){
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

void MatlBuyPolicy::set_active_type(std::string x) {
  std::set<std::string> active_options = {"Fixed", "UniformInt", "NormalInt", "Cumulative"};
  assert(active_options.count(x));
  active_type_ = x;
}

void MatlBuyPolicy::set_dormant_type(std::string x) {
  std::set<std::string> dormant_options = {"Fixed", "UniformInt", "NormalInt"};
  assert(dormant_options.count(x));
  dormant_type_ = x;
}

void MatlBuyPolicy::set_active_mean(int x) {
  assert(x > 0);
  active_mean_ = x;
}

void MatlBuyPolicy::set_dormant_mean(int x) {
  assert(x >= 0);
  dormant_mean_ = x;
}

void MatlBuyPolicy::set_active_min(int x) {
  assert(x > 0);
  active_min_ = x;
}

void MatlBuyPolicy::set_dormant_min(int x) {
  assert(x >= 0);
  dormant_min_ = x;
}

void MatlBuyPolicy::set_active_max(int x) {
  assert(x > 0);
  active_max_ = x;
}

void MatlBuyPolicy::set_dormant_max(int x) {
  assert(x >= 0);
  dormant_max_ = x;
}

void MatlBuyPolicy::set_active_stddev(double x) {
  assert(x > 0);
  active_stddev_ = x;
}

void MatlBuyPolicy::set_dormant_stddev(double x) {
  assert(x > 0);
  dormant_stddev_ = x;
}

void MatlBuyPolicy::set_default_active_dormant_behavior() {
  active_type_ = "Fixed";
  dormant_type_ = "Fixed";
  next_active_end_ = 1e299;
  next_dormant_end_ = -1;
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                   std::string name) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_default_active_dormant_behavior();
  return *this;
}

MatlBuyPolicy& MatlBuyPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                   std::string name, double throughput, std::string active_type, std::string dormant_type, int active_mean, int dormant_mean) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_throughput(throughput);
  set_active_type(active_type);
  set_dormant_type(dormant_type);
  set_active_mean(active_mean);
  set_dormant_mean(dormant_mean);
  LGH(INFO3) << "has buy policy with active = " << active_mean_ \
             << "time steps and dormant = " << dormant_mean_ << " time steps." ;
  SetNextActiveTime();
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
  set_default_active_dormant_behavior();
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

  if (current_time_ < next_active_end_) {
    // currently in the middle of active buying period
    amt = TotalQty();
  }
  else if (current_time_ < next_dormant_end_) {
    // currently in the middle of dormant period
    amt = 0;
    LGH(INFO3) << "in dormant period, no request";
  }
  else if (current_time_ == next_active_end_) {
    // finished active. starting dormancy and sample/set length of dormant period
    amt = 0;
    LGH(INFO3) << "in dormant period, no request";
    SetNextDormantTime();
    LGH(INFO4) << "Dormant period will end at time step " << next_dormant_end_;
  }
  else if (current_time_ == next_dormant_end_) {
    // finished dormant. starting buying and sample/set length of active period
    amt = TotalQty();
    SetNextActiveTime();
    LGH(INFO4) << "Active period will end at time step " << next_active_end_;
  }
  
  // if (dormant_mean_ > 0 && manager()->context()->time() % (active_mean_ + dormant_mean_) < active_mean_) {
  //   amt = TotalQty();
  // }
  // else if (dormant_mean_ == 0)
  //   amt = TotalQty();
  // else {
  //   // in dormant part of cycle, return empty portfolio
  //   amt = 0;
  //   LGH(INFO3) << "in dormant period, no request";
  // }
  if (!make_req || amt < eps())
    return ports;

  bool excl = Excl();
  double req_amt = ReqQty();
  int n_req = NReq();
  LGH(INFO3) << "requesting " << amt << " kg via " << n_req << " request(s)";

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
               << it->first.request->commodity();
    buf_->Push(it->second);
  }
}

void MatlBuyPolicy::SetNextActiveTime() {
  if (active_type_ == "Fixed" && dormant_mean_ == 0) {
    next_active_end_ = 1e299;
    next_dormant_end_ = -1;
  }
  else if (active_type_ == "Fixed") {
    next_active_end_ = manager()->context()->time() + active_mean_;
  }
  else if (active_type_ == "UniformInt") {
    next_active_end_ = manager()->context()->time() + manager()->context()->random_uniform_int(active_min_, active_max_);
  }
  else if (active_type_ == "NormalInt") {
    next_active_end_ = manager()->context()->time() + manager()->context()->random_normal_int(active_mean_, active_stddev_, active_min_, active_max_);
  }
  // if (dormant_mean_ > 0 && manager()->context()->time() % (active_mean_ + dormant_mean_) < active_mean_) {
  //   amt = TotalQty();
  // }
  // else if (dormant_mean_ == 0)
  //   amt = TotalQty();
  return;
};

void MatlBuyPolicy::SetNextDormantTime() {
  if (dormant_type_ == "Fixed") {
    next_dormant_end_ = manager()->context()->time() + dormant_mean_;
  }
  else if (dormant_type_ == "UniformInt") {
    next_dormant_end_ = manager()->context()->time() + manager()->context()->random_uniform_int(dormant_min_, dormant_max_);
  }
  else if (dormant_type_ == "NormalInt") {
    next_dormant_end_ = manager()->context()->time() + manager()->context()->random_normal_int(dormant_mean_, dormant_stddev_, dormant_min_, dormant_max_);
  }
  return;
}

}  // namespace toolkit
}  // namespace cyclus

#include "matl_sell_policy.h"

#include "error.h"

#define LG(X) LOG(LEV_##X, "selpol")
#define LGH(X)                                                    \
  LOG(LEV_##X, "selpol") << "policy " << name_ << " (agent "      \
                         << Trader::manager()->prototype() << "-" \
                         << Trader::manager()->id() << "): "

namespace cyclus {
namespace toolkit {

MatlSellPolicy::MatlSellPolicy() :
    Trader(NULL),
    name_(""),
    quantize_(0),
    throughput_(std::numeric_limits<double>::max()),
    ignore_comp_(false),
    package_(Package::unpackaged()),
    transport_unit_(TransportUnit::unrestricted()) {
  Warn<EXPERIMENTAL_WARNING>(
      "MatlSellPolicy is experimental and its API may be subject to change");
}

MatlSellPolicy::~MatlSellPolicy() {
  if (manager() != NULL)
    manager()->context()->UnregisterTrader(this);
}

void MatlSellPolicy::set_quantize(double x) {
  assert(x >= 0);
  quantize_ = x;
}

void MatlSellPolicy::set_throughput(double x) {
  assert(x >= 0);
  throughput_ = x;
}

void MatlSellPolicy::set_ignore_comp(bool x) {
  ignore_comp_ = x;
}

void MatlSellPolicy::set_package(std::string x) {
  // if no real context, only unpackaged can be used (keep default)
  if (manager() != NULL) {
    Package::Ptr pkg = manager()->context()->GetPackage(x);
    double pkg_fill = pkg->GetFillMass(quantize_);
    if ((pkg->name() != Package::unpackaged_name()) && (quantize_ > 0) &&
    (std::remainder(quantize_, pkg_fill) > 0)) { 
      std::stringstream ss;
      ss << "Quantize " << quantize_ << " is not fully packagable based on fill min/max values (" 
      << pkg->fill_min() << ", " << pkg->fill_max() << ")";
      throw ValueError(ss.str());
    }
    package_ = pkg;
  }
}

void MatlSellPolicy::set_transport_unit(std::string x) {
  if (manager() != NULL) {
    TransportUnit::Ptr tu = manager()->context()->GetTransportUnit(x);

    int num_pkgs = quantize_ / (package_->GetFillMass(quantize_));
    int max_shippable = tu->MaxShippablePackages(num_pkgs);

    if ((tu->name() != TransportUnit::unrestricted_name()) && quantize_ > 0 &&
    (max_shippable != num_pkgs))  {
      std::stringstream ss;
      ss << "Quantize " << quantize_ << " packages cannot be shipped according to transport unit fill min/max values (" << tu->fill_min() << ", "
       << tu->fill_max() << ")";
      throw ValueError(ss.str());
    }
    transport_unit_ = tu;
  }
}

MatlSellPolicy& MatlSellPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                     std::string name) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  return *this;
}

MatlSellPolicy& MatlSellPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                     std::string name, double throughput) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_throughput(throughput);
  return *this;
}

MatlSellPolicy& MatlSellPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                     std::string name, bool ignore_comp) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_ignore_comp(ignore_comp);
  return *this;
}

MatlSellPolicy& MatlSellPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                     std::string name, double throughput,
                                     bool ignore_comp) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_throughput(throughput);
  set_ignore_comp(ignore_comp);
  return *this;
}

MatlSellPolicy& MatlSellPolicy::Init(Agent* manager, ResBuf<Material>* buf,
                                     std::string name, double throughput,
                                     bool ignore_comp, double quantize,
                                     std::string package_name,
                                     std::string transport_unit_name) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_quantize(quantize);
  set_throughput(throughput);
  set_ignore_comp(ignore_comp);
  set_package(package_name);
  set_transport_unit(transport_unit_name);
  return *this;
}

MatlSellPolicy& MatlSellPolicy::Set(std::string commod) {
  commods_.insert(commod);
  return *this;
}

void MatlSellPolicy::Start() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Sell Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->RegisterTrader(this);
}

void MatlSellPolicy::Stop() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Sell Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->UnregisterTrader(this);
}


double MatlSellPolicy::Limit() const {
  double bcap = buf_->quantity();
  double limit = Excl() ?                                               \
                 quantize_ * static_cast<int>(std::floor(bcap / quantize_)) : bcap;
  return std::min(throughput_, limit);
}

std::set<BidPortfolio<Material>::Ptr> MatlSellPolicy::GetMatlBids(
    CommodMap<Material>::type& commod_requests) {
  std::set<BidPortfolio<Material>::Ptr> ports;
  

  double limit = Limit();
  if (buf_->empty() || buf_->quantity() < eps()|| limit < eps())
    return ports;
  
  BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());
  CapacityConstraint<Material> cc(limit);
  port->AddConstraint(cc);
  ports.insert(port);
  LGH(INFO3) << "bidding out " << limit << " kg";

  bool excl = Excl();
  std::string commod;
  Request<Material>* req;
  Material::Ptr m, offer;
  double qty;
  int n_full_bids;
  double bid_qty;  
  double remaining_qty;
  std::vector<double> bids;
  std::set<std::string>::iterator sit;
  std::vector<Request<Material>*>::const_iterator rit;
  for (sit = commods_.begin(); sit != commods_.end(); ++sit) {
    commod = *sit;
    if (commod_requests.count(commod) < 1)
      continue;

    const std::vector<Request<Material>*>& requests =
        commod_requests.at(commod);
    for (rit = requests.begin(); rit != requests.end(); ++rit) {
      req = *rit;
      qty = std::min(req->target()->quantity(), limit);
      bid_qty = excl ? quantize_ : package_->GetFillMass(qty);
      if (bid_qty != 0) {
        n_full_bids = static_cast<int>(std::floor(qty / bid_qty));
        bids.assign(n_full_bids, bid_qty);

        remaining_qty = fmod(qty, bid_qty);
        if (!excl && (remaining_qty > 0) && 
        (remaining_qty >= package_->fill_min())) {
          // leftover material is enough to fill one more partial package. Add
          // to bids
          bids.push_back(remaining_qty);
        }
      }
      // check transportability
      int shippable_pkgs = transport_unit_->MaxShippablePackages(bids.size());
      if (shippable_pkgs < bids.size()) {
        // can't ship all bids. Pop the extras.
        bids.erase(bids.begin() + shippable_pkgs, bids.end())
      }

      // Peek at resbuf to get current composition
      m = buf_->Peek();

      std::vector<double>::iterator bit;
      for (bit = bids.begin(); bit != bids.end(); ++bit) {
        offer = ignore_comp_ ? \
                Material::CreateUntracked(*bit, req->target()->comp()) : \
                Material::CreateUntracked(*bit, m->comp());
        port->AddBid(req, offer, this, excl);
        LG(INFO3) << "  - bid " << *bit << " kg on a request for " << commod;
      }
    } 
  }
  return ports;
}

void MatlSellPolicy::GetMatlTrades(
    const std::vector<Trade<Material> >& trades,
    std::vector<std::pair<Trade<Material>, Material::Ptr> >& responses) {
  Composition::Ptr c;
  std::vector<Trade<Material> >::const_iterator it;

  // confirm that trades are within transport unit limits
  int shippable_pkgs = transport_unit_->MaxShippablePackages(trades.size());

  double qty;

  for (it = trades.begin(); it != trades.end(); ++it) {
    if (shippable_pkgs > 0){
      qty = it->amt;
      LGH(INFO3) << " sending " << qty << " kg of " << it->request->commodity();
      Material::Ptr mat = buf_->Pop(qty, cyclus::eps_rsrc());
      Material::Ptr trade_mat;

    // don't go through packaging if you don't need to. packaging always bumps
    // resource ids and records on resources table, which is not necessary
    // when nothing is happening
    if (package_->name() != mat->package_name()) { // packaging needed
      std::vector<Material::Ptr> mat_pkgd = mat->Package<Material>(package_);

      if (mat->quantity() > eps()) {
        // push any extra material that couldn't be packaged back onto buffer
        // don't push unless there's leftover material
        buf_->Push(mat);
      }
      if (mat_pkgd.size() > 0) {
        // packaging successful
        trade_mat = mat_pkgd[0];
        shippable_pkgs-=1;
      }

    } else { // no packaging needed
      trade_mat = mat;
    }

    if (ignore_comp_) {
      trade_mat->Transmute(it->request->target()->comp());
    }
    responses.push_back(std::make_pair(*it, trade_mat));
    }
  }
}

}  // namespace toolkit
}  // namespace cyclus

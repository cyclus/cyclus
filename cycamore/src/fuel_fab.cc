#include "fuel_fab.h"

#include <sstream>

using cyclus::Material;
using cyclus::Composition;
using pyne::simple_xs;

#define SHOW(X)                                                     \
  std::cout << std::setprecision(17) << __FILE__ << ":" << __LINE__ \
            << ": " #X " = " << X << "\n"

namespace cycamore {

class FissConverter : public cyclus::Converter<cyclus::Material> {
 public:
  FissConverter(Composition::Ptr c_fill, Composition::Ptr c_fiss,
                Composition::Ptr c_topup, std::string spectrum)
      : c_fiss_(c_fiss), c_topup_(c_topup), c_fill_(c_fill), spec_(spectrum) {
    w_fiss_ = CosiWeight(c_fiss, spectrum);
    w_fill_ = CosiWeight(c_fill, spectrum);
    w_topup_ = CosiWeight(c_topup, spectrum);
  }

  virtual ~FissConverter() {}

  virtual double convert(
      cyclus::Material::Ptr m, cyclus::Arc const* a = NULL,
      cyclus::ExchangeTranslationContext<cyclus::Material> const* ctx =
          NULL) const {
    double w_tgt = CosiWeight(m->comp(), spec_);
    if (ValidWeights(w_fill_, w_tgt, w_fiss_)) {
      double frac = HighFrac(w_fill_, w_tgt, w_fiss_);
      return AtomToMassFrac(frac, c_fiss_, c_fill_) * m->quantity();
    } else if (ValidWeights(w_fiss_, w_tgt, w_topup_)) {
      // use fiss inventory as filler, and topup as fissile
      double frac = LowFrac(w_fiss_, w_tgt, w_topup_);
      return AtomToMassFrac(frac, c_fiss_, c_topup_) * m->quantity();
    } else {
      // don't bid at all
      return 1e200;
    }
  }

 private:
  std::string spec_;
  double w_fiss_;
  double w_topup_;
  double w_fill_;
  Composition::Ptr c_fiss_;
  Composition::Ptr c_fill_;
  Composition::Ptr c_topup_;
};

class FillConverter : public cyclus::Converter<cyclus::Material> {
 public:
  FillConverter(Composition::Ptr c_fill, Composition::Ptr c_fiss,
                Composition::Ptr c_topup, std::string spectrum)
      : c_fiss_(c_fiss), c_topup_(c_topup), c_fill_(c_fill), spec_(spectrum) {
    w_fiss_ = CosiWeight(c_fiss, spectrum);
    w_fill_ = CosiWeight(c_fill, spectrum);
    w_topup_ = CosiWeight(c_topup, spectrum);
  }

  virtual ~FillConverter() {}

  virtual double convert(
      cyclus::Material::Ptr m, cyclus::Arc const* a = NULL,
      cyclus::ExchangeTranslationContext<cyclus::Material> const* ctx =
          NULL) const {
    double w_tgt = CosiWeight(m->comp(), spec_);
    if (ValidWeights(w_fill_, w_tgt, w_fiss_)) {
      double frac = LowFrac(w_fill_, w_tgt, w_fiss_);
      return AtomToMassFrac(frac, c_fill_, c_fiss_) * m->quantity();
    } else if (ValidWeights(w_fiss_, w_tgt, w_topup_)) {
      // switched fissile inventory to filler so don't need any filler inventory
      return 0;
    } else {
      // don't bid at all
      return 1e200;
    }
  }

 private:
  std::string spec_;
  double w_fiss_;
  double w_topup_;
  double w_fill_;
  Composition::Ptr c_fiss_;
  Composition::Ptr c_fill_;
  Composition::Ptr c_topup_;
};

class TopupConverter : public cyclus::Converter<cyclus::Material> {
 public:
  TopupConverter(Composition::Ptr c_fill, Composition::Ptr c_fiss,
                 Composition::Ptr c_topup, std::string spectrum)
      : c_fiss_(c_fiss), c_topup_(c_topup), c_fill_(c_fill), spec_(spectrum) {
    w_fiss_ = CosiWeight(c_fiss, spectrum);
    w_fill_ = CosiWeight(c_fill, spectrum);
    w_topup_ = CosiWeight(c_topup, spectrum);
  }

  virtual ~TopupConverter() {}

  virtual double convert(
      cyclus::Material::Ptr m, cyclus::Arc const* a = NULL,
      cyclus::ExchangeTranslationContext<cyclus::Material> const* ctx =
          NULL) const {
    double w_tgt = CosiWeight(m->comp(), spec_);
    if (ValidWeights(w_fill_, w_tgt, w_fiss_)) {
      return 0;
    } else if (ValidWeights(w_fiss_, w_tgt, w_topup_)) {
      // switched fissile inventory to filler and topup as fissile
      double frac = HighFrac(w_fiss_, w_tgt, w_topup_);
      return AtomToMassFrac(frac, c_topup_, c_fiss_) * m->quantity();
    } else {
      // don't bid at all
      return 1e200;
    }
  }

 private:
  std::string spec_;
  double w_fiss_;
  double w_topup_;
  double w_fill_;
  Composition::Ptr c_fiss_;
  Composition::Ptr c_fill_;
  Composition::Ptr c_topup_;
};

FuelFab::FuelFab(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      fill_size(0),
      fiss_size(0),
      throughput(0),
      latitude(0.0),
      longitude(0.0),
      coordinates(latitude, longitude) {}

void FuelFab::EnterNotify() {
  cyclus::Facility::EnterNotify();

  if (fiss_commod_prefs.empty()) {
    for (int i = 0; i < fiss_commods.size(); i++) {
      fiss_commod_prefs.push_back(cyclus::kDefaultPref);
    }
  } else if (fiss_commod_prefs.size() != fiss_commods.size()) {
    std::stringstream ss;
    ss << "prototype '" << prototype() << "' has " << fiss_commod_prefs.size()
       << " fiss_commod_prefs vals, expected " << fiss_commods.size();
    throw cyclus::ValidationError(ss.str());
  }

  if (fill_commod_prefs.empty()) {
    for (int i = 0; i < fill_commods.size(); i++) {
      fill_commod_prefs.push_back(cyclus::kDefaultPref);
    }
  } else if (fill_commod_prefs.size() != fill_commods.size()) {
    std::stringstream ss;
    ss << "prototype '" << prototype() << "' has " << fill_commod_prefs.size()
       << " fill_commod_prefs vals, expected " << fill_commods.size();
    throw cyclus::ValidationError(ss.str());
  }
  RecordPosition();
}

std::set<cyclus::RequestPortfolio<Material>::Ptr> FuelFab::GetMatlRequests() {
  using cyclus::RequestPortfolio;

  std::set<RequestPortfolio<Material>::Ptr> ports;

  bool exclusive = false;

  if (fiss.space() > cyclus::eps_rsrc()) {
    RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());

    Material::Ptr m = cyclus::NewBlankMaterial(fiss.space());
    if (!fiss_recipe.empty()) {
      Composition::Ptr c = context()->GetRecipe(fiss_recipe);
      m = Material::CreateUntracked(fiss.space(), c);
    }

    std::vector<cyclus::Request<Material>*> reqs;
    for (int i = 0; i < fiss_commods.size(); i++) {
      std::string commod = fiss_commods[i];
      double pref = fiss_commod_prefs[i];
      reqs.push_back(port->AddRequest(m, this, commod, pref, exclusive));
      req_inventories_[reqs.back()] = "fiss";
    }
    port->AddMutualReqs(reqs);
    ports.insert(port);
  }

  if (fill.space() > cyclus::eps_rsrc()) {
    RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());

    Material::Ptr m = cyclus::NewBlankMaterial(fill.space());
    if (!fill_recipe.empty()) {
      Composition::Ptr c = context()->GetRecipe(fill_recipe);
      m = Material::CreateUntracked(fill.space(), c);
    }

    std::vector<cyclus::Request<Material>*> reqs;
    for (int i = 0; i < fill_commods.size(); i++) {
      std::string commod = fill_commods[i];
      double pref = fill_commod_prefs[i];
      reqs.push_back(port->AddRequest(m, this, commod, pref, exclusive));
      req_inventories_[reqs.back()] = "fill";
    }
    port->AddMutualReqs(reqs);
    ports.insert(port);
  }

  if (topup.space() > cyclus::eps_rsrc()) {
    RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());

    Material::Ptr m = cyclus::NewBlankMaterial(topup.space());
    if (!topup_recipe.empty()) {
      Composition::Ptr c = context()->GetRecipe(topup_recipe);
      m = Material::CreateUntracked(topup.space(), c);
    }
    cyclus::Request<Material>* r =
        port->AddRequest(m, this, topup_commod, topup_pref, exclusive);
    req_inventories_[r] = "topup";
    ports.insert(port);
  }

  return ports;
}

bool Contains(std::vector<std::string> vec, std::string s) {
  for (int i = 0; i < vec.size(); i++) {
    if (vec[i] == s) {
      return true;
    }
  }
  return false;
}

void FuelFab::AcceptMatlTrades(
    const std::vector<std::pair<cyclus::Trade<Material>, Material::Ptr> >&
        responses) {
  std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                        cyclus::Material::Ptr> >::const_iterator trade;

  for (trade = responses.begin(); trade != responses.end(); ++trade) {
    std::string commod = trade->first.request->commodity();
    double req_qty = trade->first.request->target()->quantity();
    cyclus::Request<Material>* req = trade->first.request;
    Material::Ptr m = trade->second;
    if (req_inventories_[req] == "fill") {
      fill.Push(m);
    } else if (req_inventories_[req] == "topup") {
      topup.Push(m);
    } else if (req_inventories_[req] == "fiss") {
      fiss.Push(m);
    } else {
      throw cyclus::ValueError("cycamore::FuelFab was overmatched on requests");
    }
  }

  req_inventories_.clear();

  // IMPORTANT - each buffer needs to be a single homogenous composition or
  // the inventory mixing constraints for bids don't work
  if (fill.count() > 1) {
    fill.Push(cyclus::toolkit::Squash(fill.PopN(fill.count())));
  }
  if (fiss.count() > 1) {
    fiss.Push(cyclus::toolkit::Squash(fiss.PopN(fiss.count())));
  }
  if (topup.count() > 1) {
    topup.Push(cyclus::toolkit::Squash(topup.PopN(topup.count())));
  }
}

std::set<cyclus::BidPortfolio<Material>::Ptr> FuelFab::GetMatlBids(
    cyclus::CommodMap<Material>::type& commod_requests) {
  using cyclus::BidPortfolio;

  std::set<BidPortfolio<Material>::Ptr> ports;
  std::vector<cyclus::Request<Material>*>& reqs = commod_requests[outcommod];

  if (throughput == 0) {
    return ports;
  } else if (reqs.size() == 0) {
    return ports;
  }

  double w_fill = 0;
  Composition::Ptr
      c_fill;  // no default needed - this is non-optional parameter
  if (fill.count() > 0) {
    c_fill = fill.Peek()->comp();
    w_fill = CosiWeight(c_fill, spectrum);
  } else {
    c_fill = context()->GetRecipe(fill_recipe);
    w_fill = CosiWeight(c_fill, spectrum);
  }

  double w_topup = 0;
  Composition::Ptr c_topup = c_fill;
  if (topup.count() > 0) {
    c_topup = topup.Peek()->comp();
    w_topup = CosiWeight(c_topup, spectrum);
  } else if (!topup_recipe.empty()) {
    c_topup = context()->GetRecipe(topup_recipe);
    w_topup = CosiWeight(c_topup, spectrum);
  }

  double w_fiss =
      w_fill;  // this allows trading just fill with no fiss inventory
  Composition::Ptr c_fiss = c_fill;
  if (fiss.count() > 0) {
    c_fiss = fiss.Peek()->comp();
    w_fiss = CosiWeight(c_fiss, spectrum);
  } else if (!fiss_recipe.empty()) {
    c_fiss = context()->GetRecipe(fiss_recipe);
    w_fiss = CosiWeight(c_fiss, spectrum);
  }

  BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());
  for (int j = 0; j < reqs.size(); j++) {
    cyclus::Request<Material>* req = reqs[j];

    Composition::Ptr tgt = req->target()->comp();
    double w_tgt = CosiWeight(tgt, spectrum);
    double tgt_qty = req->target()->quantity();
    if (ValidWeights(w_fill, w_tgt, w_fiss)) {
      double fiss_frac = HighFrac(w_fill, w_tgt, w_fiss);
      double fill_frac = 1 - fiss_frac;
      fiss_frac = AtomToMassFrac(fiss_frac, c_fiss, c_fill);
      fill_frac = AtomToMassFrac(fill_frac, c_fill, c_fiss);
      Material::Ptr m1 = Material::CreateUntracked(fiss_frac * tgt_qty, c_fiss);
      Material::Ptr m2 = Material::CreateUntracked(fill_frac * tgt_qty, c_fill);
      m1->Absorb(m2);

      bool exclusive = false;
      port->AddBid(req, m1, this, exclusive);
    } else if (topup.count() > 0 && ValidWeights(w_fiss, w_tgt, w_topup)) {
      // only bid with topup if we have filler - otherwise we might be able to
      // meet target with filler when we get it. we should only use topup
      // when the fissile has too poor neutronics.
      double topup_frac = HighFrac(w_fiss, w_tgt, w_topup);
      double fiss_frac = 1 - topup_frac;
      fiss_frac = AtomToMassFrac(fiss_frac, c_fiss, c_topup);
      topup_frac = AtomToMassFrac(topup_frac, c_topup, c_fiss);
      Material::Ptr m1 =
          Material::CreateUntracked(topup_frac * tgt_qty, c_topup);
      Material::Ptr m2 = Material::CreateUntracked(fiss_frac * tgt_qty, c_fiss);
      m1->Absorb(m2);

      bool exclusive = false;
      port->AddBid(req, m1, this, exclusive);
    } else if (fiss.count() > 0 && fill.count() > 0 ||
               fiss.count() > 0 && topup.count() > 0) {
      // else can't meet the target weight - don't bid.  Just a plain else
      // doesn't work because we set w_fiss = w_fill if we don't have any fiss
      // or fill inventory.
      std::stringstream ss;
      ss << "prototype '" << prototype()
         << "': Input stream weights/reactivity do not span "
            "the requested material weight.";
      cyclus::Warn<cyclus::VALUE_WARNING>(ss.str());
    }
  }

  cyclus::Converter<Material>::Ptr fissconv(
      new FissConverter(c_fill, c_fiss, c_topup, spectrum));
  cyclus::Converter<Material>::Ptr fillconv(
      new FillConverter(c_fill, c_fiss, c_topup, spectrum));
  cyclus::Converter<Material>::Ptr topupconv(
      new TopupConverter(c_fill, c_fiss, c_topup, spectrum));
  // important! - the std::max calls prevent CapacityConstraint throwing a zero
  // cap exception
  cyclus::CapacityConstraint<Material> fissc(std::max(fiss.quantity(), 1e-10),
                                             fissconv);
  cyclus::CapacityConstraint<Material> fillc(std::max(fill.quantity(), 1e-10),
                                             fillconv);
  cyclus::CapacityConstraint<Material> topupc(std::max(topup.quantity(), 1e-10),
                                              topupconv);
  port->AddConstraint(fillc);
  port->AddConstraint(fissc);
  port->AddConstraint(topupc);

  cyclus::CapacityConstraint<Material> cc(throughput);
  port->AddConstraint(cc);
  ports.insert(port);
  return ports;
}

void FuelFab::GetMatlTrades(
    const std::vector<cyclus::Trade<Material> >& trades,
    std::vector<std::pair<cyclus::Trade<Material>, Material::Ptr> >&
        responses) {
  using cyclus::Trade;

  // guard against cases where a buffer is empty - this is okay because some
  // trades may not need that particular buffer.
  double w_fill = 0;
  if (fill.count() > 0) {
    w_fill = CosiWeight(fill.Peek()->comp(), spectrum);
  }
  double w_topup = 0;
  if (topup.count() > 0) {
    w_topup = CosiWeight(topup.Peek()->comp(), spectrum);
  }
  double w_fiss = 0;
  if (fiss.count() > 0) {
    w_fiss = CosiWeight(fiss.Peek()->comp(), spectrum);
  }

  std::vector<cyclus::Trade<cyclus::Material> >::const_iterator it;
  double tot = 0;
  for (int i = 0; i < trades.size(); i++) {
    Material::Ptr tgt = trades[i].request->target();

    double w_tgt = CosiWeight(tgt->comp(), spectrum);
    double qty = trades[i].amt;
    double wfiss = w_fiss;

    tot += qty;
    if (tot > throughput + cyclus::eps_rsrc()) {
      std::stringstream ss;
      ss << "FuelFab was matched above throughput limit: " << tot << " > "
         << throughput;
      throw cyclus::ValueError(ss.str());
    }

    if (fiss.count() == 0) {
      // use straight filler to satisfy this request
      double fillqty = qty;
      if (std::abs(fillqty - fill.quantity()) < cyclus::eps_rsrc()) {
        fillqty = std::min(fill.quantity(), qty);
      }
      responses.push_back(
          std::make_pair(trades[i], fill.Pop(fillqty, cyclus::eps_rsrc())));
    } else if (fill.count() == 0 && ValidWeights(w_fill, w_tgt, w_fiss)) {
      // use straight fissile to satisfy this request
      double fissqty = qty;
      if (std::abs(fissqty - fiss.quantity()) < cyclus::eps_rsrc()) {
        fissqty = std::min(fiss.quantity(), qty);
      }
      responses.push_back(
          std::make_pair(trades[i], fiss.Pop(fissqty, cyclus::eps_rsrc())));
    } else if (ValidWeights(w_fill, w_tgt, w_fiss)) {
      double fiss_frac = HighFrac(w_fill, w_tgt, w_fiss);
      double fill_frac = LowFrac(w_fill, w_tgt, w_fiss);
      fiss_frac =
          AtomToMassFrac(fiss_frac, fiss.Peek()->comp(), fill.Peek()->comp());
      fill_frac =
          AtomToMassFrac(fill_frac, fill.Peek()->comp(), fiss.Peek()->comp());

      double fissqty = fiss_frac * qty;
      if (std::abs(fissqty - fiss.quantity()) < cyclus::eps_rsrc()) {
        fissqty = std::min(fiss.quantity(), fiss_frac * qty);
      }
      double fillqty = fill_frac * qty;
      if (std::abs(fillqty - fill.quantity()) < cyclus::eps_rsrc()) {
        fillqty = std::min(fill.quantity(), fill_frac * qty);
      }

      Material::Ptr m = fiss.Pop(fissqty, cyclus::eps_rsrc());
      // this if block prevents zero qty ResBuf pop exceptions
      if (fill_frac > 0) {
        m->Absorb(fill.Pop(fillqty, cyclus::eps_rsrc()));
      }
      responses.push_back(std::make_pair(trades[i], m));
    } else {
      double topup_frac = HighFrac(w_fiss, w_tgt, w_topup);
      double fiss_frac = 1 - topup_frac;
      topup_frac =
          AtomToMassFrac(topup_frac, topup.Peek()->comp(), fiss.Peek()->comp());
      fiss_frac =
          AtomToMassFrac(fiss_frac, fiss.Peek()->comp(), topup.Peek()->comp());

      double fissqty = fiss_frac * qty;
      if (std::abs(fissqty - fiss.quantity()) < cyclus::eps_rsrc()) {
        fissqty = std::min(fiss.quantity(), fiss_frac * qty);
      }
      double topupqty = topup_frac * qty;
      if (std::abs(topupqty - topup.quantity()) < cyclus::eps_rsrc()) {
        topupqty = std::min(topup.quantity(), topup_frac * qty);
      }

      Material::Ptr m = fiss.Pop(fissqty, cyclus::eps_rsrc());
      // this if block prevents zero qty ResBuf pop exceptions
      if (topup_frac > 0) {
        m->Absorb(topup.Pop(topupqty, cyclus::eps_rsrc()));
      }
      responses.push_back(std::make_pair(trades[i], m));
    }
  }
}

void FuelFab::RecordPosition() {
  std::string specification = this->spec();
  context()
      ->NewDatum("AgentPosition")
      ->AddVal("Spec", specification)
      ->AddVal("Prototype", this->prototype())
      ->AddVal("AgentId", id())
      ->AddVal("Latitude", latitude)
      ->AddVal("Longitude", longitude)
      ->Record();
}

extern "C" cyclus::Agent* ConstructFuelFab(cyclus::Context* ctx) {
  return new FuelFab(ctx);
}

// Returns the weight of c using 1 group cross sections of type spectrum
// which must be one of:
//
//     * thermal
//     * thermal_maxwell_ave
//     * fission_spectrum_ave
//     * resonance_integral
//     * fourteen_MeV
//
// The weight is calculated as "(nu*sigma_f - sigma_a) * N".  Since weights
// are computed based on nuclide atom fractions, corresponding computed
// material/mixing fractions will also be atom-based naturally and will need
// to be converted to mass-based for actual material object mixing.
double CosiWeight(cyclus::Composition::Ptr c, const std::string& spectrum) {
  cyclus::CompMap cm = c->atom();
  cyclus::compmath::Normalize(&cm);

  if (spectrum == "thermal") {
    double nu_pu239 = 2.85;
    double nu_u233 = 2.5;
    double nu_u235 = 2.43;
    double nu_u238 = 0;
    double nu_pu241 = nu_pu239;

    static std::map<int, double> absorb_xs;
    static std::map<int, double> fiss_xs;
    static double p_u238 = 0;
    static double p_pu239 = 0;
    if (p_u238 == 0) {
      double fiss_u238 = simple_xs(922380000, "fission", "thermal");
      double absorb_u238 = simple_xs(922380000, "absorption", "thermal");
      p_u238 = nu_u238 * fiss_u238 - absorb_u238;

      double fiss_pu239 = simple_xs(942390000, "fission", "thermal");
      double absorb_pu239 = simple_xs(942390000, "absorption", "thermal");
      p_pu239 = nu_pu239 * fiss_pu239 - absorb_pu239;
    }

    cyclus::CompMap::iterator it;
    double w = 0;
    for (it = cm.begin(); it != cm.end(); ++it) {
      cyclus::Nuc nuc = it->first;
      double nu = 0;
      if (nuc == 922350000) {
        nu = nu_u235;
      } else if (nuc == 922330000) {
        nu = nu_u233;
      } else if (nuc == 942390000) {
        nu = nu_pu239;
      } else if (nuc == 942410000) {
        nu = nu_pu241;
      }

      double fiss = 0;
      double absorb = 0;
      if (absorb_xs.count(nuc) == 0) {
        try {
          fiss = simple_xs(nuc, "fission", "thermal");
          absorb = simple_xs(nuc, "absorption", "thermal");
          absorb_xs[nuc] = absorb;
          fiss_xs[nuc] = fiss;
        } catch (pyne::InvalidSimpleXS err) {
          fiss = 0;
          absorb = 0;
        }
      } else {
        fiss = fiss_xs[nuc];
        absorb = absorb_xs[nuc];
      }

      double p = nu * fiss - absorb;
      w += it->second * (p - p_u238) / (p_pu239 - p_u238);
    }
    return w;
  } else if (spectrum == "fission_spectrum_ave") {
    double nu_pu239 = 3.1;
    double nu_u233 = 2.63;
    double nu_u235 = 2.58;
    double nu_u238 = 0;
    double nu_pu241 = nu_pu239;

    static std::map<int, double> absorb_xs;
    static std::map<int, double> fiss_xs;
    static double p_u238 = 0;
    static double p_pu239 = 0;
    if (p_u238 == 0) {
      double fiss_u238 =
          simple_xs(922380000, "fission", "fission_spectrum_ave");
      double absorb_u238 =
          simple_xs(922380000, "absorption", "fission_spectrum_ave");
      p_u238 = nu_u238 * fiss_u238 - absorb_u238;

      double fiss_pu239 =
          simple_xs(942390000, "fission", "fission_spectrum_ave");
      double absorb_pu239 =
          simple_xs(942390000, "absorption", "fission_spectrum_ave");
      p_pu239 = nu_pu239 * fiss_pu239 - absorb_pu239;
    }

    cyclus::CompMap::iterator it;
    double w = 0;
    for (it = cm.begin(); it != cm.end(); ++it) {
      cyclus::Nuc nuc = it->first;
      double nu = 0;
      if (nuc == 922350000) {
        nu = nu_u235;
      } else if (nuc == 922330000) {
        nu = nu_u233;
      } else if (nuc == 942390000) {
        nu = nu_pu239;
      } else if (nuc == 942410000) {
        nu = nu_pu241;
      }

      double fiss = 0;
      double absorb = 0;
      if (absorb_xs.count(nuc) == 0) {
        try {
          fiss = simple_xs(nuc, "fission", "fission_spectrum_ave");
          absorb = simple_xs(nuc, "absorption", "fission_spectrum_ave");
          absorb_xs[nuc] = absorb;
          fiss_xs[nuc] = fiss;
        } catch (pyne::InvalidSimpleXS err) {
          fiss = 0;
          absorb = 0;
        }
      } else {
        fiss = fiss_xs[nuc];
        absorb = absorb_xs[nuc];
      }

      double p = nu * fiss - absorb;
      w += it->second * (p - p_u238) / (p_pu239 - p_u238);
    }
    return w;
  } else {
    double nu_pu239 = 3.1;
    double nu_u233 = 2.63;
    double nu_u235 = 2.58;
    double nu_u238 = 0;
    double nu_pu241 = nu_pu239;

    double fiss_u238 = simple_xs(922380000, "fission", spectrum);
    double absorb_u238 = simple_xs(922380000, "absorption", spectrum);
    double p_u238 = nu_u238 * fiss_u238 - absorb_u238;

    double fiss_pu239 = simple_xs(942390000, "fission", spectrum);
    double absorb_pu239 = simple_xs(942390000, "absorption", spectrum);
    double p_pu239 = nu_pu239 * fiss_pu239 - absorb_pu239;

    cyclus::CompMap::iterator it;
    double w = 0;
    for (it = cm.begin(); it != cm.end(); ++it) {
      cyclus::Nuc nuc = it->first;
      double nu = 0;
      if (nuc == 922350000) {
        nu = nu_u235;
      } else if (nuc == 922330000) {
        nu = nu_u233;
      } else if (nuc == 942390000) {
        nu = nu_pu239;
      } else if (nuc == 942410000) {
        nu = nu_pu241;
      }

      double fiss = 0;
      double absorb = 0;
      try {
        fiss = simple_xs(nuc, "fission", spectrum);
        absorb = simple_xs(nuc, "absorption", spectrum);
      } catch (pyne::InvalidSimpleXS err) {
        fiss = 0;
        absorb = 0;
      }

      double p = nu * fiss - absorb;
      w += it->second * (p - p_u238) / (p_pu239 - p_u238);
    }
    return w;
  }
}

// Convert an atom frac (n1/(n1+n2) to a mass frac (m1/(m1+m2) given
// corresponding compositions c1 and c2.
double AtomToMassFrac(double atomfrac, Composition::Ptr c1,
                      Composition::Ptr c2) {
  cyclus::CompMap n1 = c1->atom();
  cyclus::CompMap n2 = c2->atom();
  cyclus::compmath::Normalize(&n1, atomfrac);
  cyclus::compmath::Normalize(&n2, 1 - atomfrac);

  cyclus::CompMap::iterator it;

  double mass1 = 0;
  for (it = n1.begin(); it != n1.end(); ++it) {
    mass1 += it->second * pyne::atomic_mass(it->first);
  }

  double mass2 = 0;
  for (it = n2.begin(); it != n2.end(); ++it) {
    mass2 += it->second * pyne::atomic_mass(it->first);
  }

  return mass1 / (mass1 + mass2);
}

double HighFrac(double w_low, double w_target, double w_high, double eps) {
  if (!ValidWeights(w_low, w_target, w_high)) {
    throw cyclus::ValueError("low and high weights cannot meet target");
  } else if (w_low == w_high && w_target == w_low) {
    return 1;
  }
  double f = std::abs((w_target - w_low) / (w_high - w_low));
  if (1 - f < eps) {
    return 1;
  } else if (f < eps) {
    return 0;
  }
  return f;
}

double LowFrac(double w_low, double w_target, double w_high, double eps) {
  return 1 - HighFrac(w_low, w_target, w_high, eps);
}

// Returns true if the given weights can be used to linearly interpolate valid
// mixing fractions of the filler and fissile streams to hit the target.
bool ValidWeights(double w_low, double w_target, double w_high) {
  // w_tgt must be in between w_fill and w_fiss for the equivalence
  // interpolation to work.
  return w_low <= w_target && w_target <= w_high;
}

}  // namespace cycamore

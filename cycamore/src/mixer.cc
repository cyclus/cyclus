#include <sstream>

#include "mixer.h"

namespace cycamore {

Mixer::Mixer(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      throughput(0),
      latitude(0.0),
      longitude(0.0),
      coordinates(latitude, longitude) {
  cyclus::Warn<cyclus::EXPERIMENTAL_WARNING>(
      "the Mixer archetype is experimental");
  RecordPosition();
}

cyclus::Inventories Mixer::SnapshotInv() {
  cyclus::Inventories invs;

  // these inventory names are intentionally convoluted so as to not clash
  // with the user-specified stream commods that are used as the Mixer
  // streams inventory names.
  invs["output-inv-name"] = output.PopNRes(output.count());
  output.Push(invs["output-inv-name"]);

  std::map<std::string, cyclus::toolkit::ResBuf<cyclus::Material> >::iterator
      it;
  for (it = streambufs.begin(); it != streambufs.end(); ++it) {
    invs[it->first] = it->second.PopNRes(it->second.count());
    it->second.Push(invs[it->first]);
  }
  return invs;
}

void Mixer::InitInv(cyclus::Inventories& inv) {
  inv["output-inv-name"] = output.PopNRes(output.count());
  output.Push(inv["output-inv-name"]);

  cyclus::Inventories::iterator it;
  for (it = inv.begin(); it != inv.end(); ++it) {
    streambufs[it->first].Push(it->second);
  }
}

void Mixer::EnterNotify() {
  cyclus::Facility::EnterNotify();

  mixing_ratios.clear();
  in_buf_sizes.clear();
  in_commods.clear();

  // initialisation internal variable
  for (int i = 0; i < streams_.size(); i++) {
    mixing_ratios.push_back(streams_[i].first.first);
    in_buf_sizes.push_back(streams_[i].first.second);

    std::string name = "in_stream_" + std::to_string(i);
    double cap = in_buf_sizes[i];
    if (cap >= 0) {
      streambufs[name].capacity(cap);
    }
    in_commods.push_back(streams_[i].second);
  }

  // ratio normalisation
  if (mixing_ratios.size() != in_commods.size()) {
    std::stringstream ss;
    ss << "prototype '" << prototype() << "' has " << mixing_ratios.size()
       << " commodity fraction values, expected " << in_commods.size();
    throw cyclus::ValidationError(ss.str());

  } else {
    double frac_sum = 0.0;
    for (int i = 0; i < mixing_ratios.size(); i++) frac_sum += mixing_ratios[i];

    if (frac_sum != 1.0) {
      std::stringstream ss;
      ss << "prototype '" << prototype() << "': the sum of mixing fractions is "
                                            "not 1, renormalization will be "
                                            "done.";
      cyclus::Warn<cyclus::VALUE_WARNING>(ss.str());
    }
    if (frac_sum != 0) {
      for (int i = 0; i < mixing_ratios.size(); i++) {
        mixing_ratios[i] *= 1.0 / frac_sum;
      }
    } else {
      for (int i = 0; i < mixing_ratios.size(); i++) {
        mixing_ratios[i] = 1.0 / (mixing_ratios.size());
      }
    }
  }

  sell_policy.Init(this, &output, "output").Set(out_commod).Start();
}

void Mixer::Tick() {
  using cyclus::toolkit::RecordTimeSeries;
  if (output.quantity() < output.capacity()) {
    double tgt_qty = output.space();

    for (int i = 0; i < mixing_ratios.size(); i++) {
      std::string name = "in_stream_" + std::to_string(i);
      tgt_qty =
          std::min(tgt_qty, streambufs[name].quantity() / mixing_ratios[i]);
    }

    tgt_qty = std::min(tgt_qty, throughput);

    if (tgt_qty > 0) {
      cyclus::Material::Ptr m;
      for (int i = 0; i < mixing_ratios.size(); i++) {
        std::string name = "in_stream_" + std::to_string(i);
        double pop_qty = mixing_ratios[i] * tgt_qty;
        if (i == 0) {
          m = streambufs[name].Pop(pop_qty, cyclus::eps_rsrc());
        } else {
          cyclus::Material::Ptr m_ =
              streambufs[name].Pop(pop_qty, cyclus::eps_rsrc());
          m->Absorb(m_);
        }
      }
      output.Push(m);
    }
  }
  cyclus::toolkit::RecordTimeSeries<double>("supply"+out_commod, this, output.quantity());
}

std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
Mixer::GetMatlRequests() {
  using cyclus::RequestPortfolio;
  using cyclus::toolkit::RecordTimeSeries;

  for (int i = 0; i < mixing_ratios.size(); i++)
  {
    std::string name = "in_stream_" + std::to_string(i);
    std::map<std::string, double>::iterator it;
    std::map<std::string, double>::iterator max_it = in_commods[i].begin();
    double prev_pref = 0;
    for (it = in_commods[i].begin(); it != in_commods[i].end(); it++)
    {
      cyclus::toolkit::RecordTimeSeries<double>("demand" + it->first, this,
                                                streambufs[name].space());
    }
  }

  std::set<RequestPortfolio<cyclus::Material>::Ptr> ports;

  for (int i = 0; i < in_commods.size(); i++) {
    std::string name = "in_stream_" + std::to_string(i);

    if (streambufs[name].space() > cyclus::eps_rsrc()) {
      RequestPortfolio<cyclus::Material>::Ptr port(
          new RequestPortfolio<cyclus::Material>());

      cyclus::Material::Ptr m;
      m = cyclus::NewBlankMaterial(streambufs[name].space());

      std::vector<cyclus::Request<cyclus::Material>*> reqs;

      std::map<std::string, double>::iterator it;
      for (it = in_commods[i].begin() ; it != in_commods[i].end(); it++) {
        std::string commod = it->first;
        double pref = it->second;
        reqs.push_back(port->AddRequest(m, this, commod , pref, false));
        req_inventories_[reqs.back()] = name;
      }
      port->AddMutualReqs(reqs);
      ports.insert(port);
    }
  }
  return ports;
}

void Mixer::AcceptMatlTrades(
    const std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                                cyclus::Material::Ptr> >& responses) {
  std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                        cyclus::Material::Ptr> >::const_iterator trade;

  for (trade = responses.begin(); trade != responses.end(); ++trade) {
    cyclus::Request<cyclus::Material>* req = trade->first.request;
    cyclus::Material::Ptr m = trade->second;

    std::string name = req_inventories_[req];
    bool assigned = false;
    std::map<std::string, cyclus::toolkit::ResBuf<cyclus::Material> >::iterator
        it;

    for (it = streambufs.begin(); it != streambufs.end(); it++) {
      if (name == it->first) {
        it->second.Push(m);
        assigned = true;
        break;
      }
    }
    if (!assigned) {
      throw cyclus::ValueError("cycamore::Mixer was overmatched on requests");
    }
  }

  req_inventories_.clear();
}

void Mixer::RecordPosition() {
  std::string specification = spec();
  context()
      ->NewDatum("AgentPosition")
      ->AddVal("Spec", specification)
      ->AddVal("Prototype", this->prototype())
      ->AddVal("AgentId", id())
      ->AddVal("Latitude", latitude)
      ->AddVal("Longitude", longitude)
      ->Record();
}

extern "C" cyclus::Agent* ConstructMixer(cyclus::Context* ctx) {
  return new Mixer(ctx);
}
}

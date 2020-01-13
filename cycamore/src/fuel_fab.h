#ifndef CYCAMORE_SRC_FUEL_FAB_H_
#define CYCAMORE_SRC_FUEL_FAB_H_

#include <string>
#include "cyclus.h"
#include "cycamore_version.h"

namespace cycamore {

/// FuelFab takes in 2 streams of material and mixes them in ratios in order to
/// supply material that matches some neutronics properties of reqeusted
/// material.  It uses an equivalence type method [1]
/// inspired by a similar approach in the COSI fuel cycle simulator.
///
/// The FuelFab has 3 input inventories: fissile stream, filler stream, and an
/// optional top-up inventory.  All materials received into each inventory are
/// always combined into a single material (i.e. a single fissile material, a
/// single filler material, etc.).  The input streams and requested fuel
/// composition are each assigned weights based on summing:
///
///     N * (p_i - p_U238) / (p_Pu239 - p_U238)
///
/// for each nuclide where:
///
///     - p = nu*sigma_f - sigma_a   for the nuclide
///     - p_U238 is p for pure U238
///     - p_Pu239 is p for pure Pu239
///     - N is the nuclide's atom fraction
///     - nu is the average # neutrons per fission
///     - sigma_f is the microscopic fission cross-section
///     - sigma_a is the microscopic neutron absorption cross-section
///
/// The cross sections are from the simple cross section library in PyNE. They
/// can be set to either a thermal or fast neutron spectrum.  A linear
/// interpolation is performed using the weights of the fissile, filler, and
/// target streams. The interpolation is used to compute a mixing ratio of the
/// input streams that matches the target weight.  In the event that the target
/// weight is higher than the fissile stream weight, the FuelFab will attempt
/// to use the top-up and fissile input streams together instead of the fissile
/// and filler streams.  All supplied material will always have the same weight
/// as the requested material.
///
/// The supplying of mixed material is constrained by available inventory
/// quantities and a per time step throughput limit.  Requests for fuel
/// material larger than the throughput can never be met.  Fissile inventory
/// can be requested/received via one or more commodities.  The DRE request
/// preference for each of these commodities can also optionally be specified.
/// By default, the top-up inventory size is zero, and it is not used for
/// mixing.
///
/// @code
/// [1] Baker, A. R., and R. W. Ross. "Comparison of the value of plutonium and
///     uranium isotopes in fast reactors." Proceedings of the Conference on
///     Breeding. Economics, and Safety in Large Fast Power Reactors. 1963.
/// @endcode
class FuelFab
  : public cyclus::Facility,
    public cyclus::toolkit::Position {
#pragma cyclus note { \
"niche": "fabrication", \
"doc": \
  "FuelFab takes in 2 streams of material and mixes them in ratios in order to" \
  " supply material that matches some neutronics properties of reqeusted" \
  " material.  It uses an equivalence type method [1]" \
  " inspired by a similar approach in the COSI fuel cycle simulator." \
  "\n\n" \
  "The FuelFab has 3 input inventories: fissile stream, filler stream, and an" \
  " optional top-up inventory.  All materials received into each inventory are" \
  " always combined into a single material (i.e. a single fissile material, a" \
  " single filler material, etc.).  The input streams and requested fuel" \
  " composition are each assigned weights based on summing:" \
  "\n\n" \
  "    N * (p_i - p_U238) / (p_Pu239 - p_U238)" \
  "\n\n" \
  "for each nuclide where:" \
  "\n" \
  "\n    - p = nu*sigma_f - sigma_a   for the nuclide" \
  "\n    - p_U238 is p for pure U238" \
  "\n    - p_Pu239 is p for pure Pu239" \
  "\n    - N is the nuclide's atom fraction" \
  "\n    - nu is the average # neutrons per fission" \
  "\n    - sigma_f is the microscopic fission cross-section" \
  "\n    - sigma_a is the microscopic neutron absorption cross-section" \
  "\n\n" \
  "The cross sections are from the simple cross section library in PyNE. They" \
  " can be set to either a thermal or fast neutron spectrum.  A linear" \
  " interpolation is performed using the weights of the fissile, filler, and" \
  " target streams. The interpolation is used to compute a mixing ratio of the" \
  " input streams that matches the target weight.  In the event that the target" \
  " weight is higher than the fissile stream weight, the FuelFab will attempt" \
  " to use the top-up and fissile input streams together instead of the fissile" \
  " and filler streams.  All supplied material will always have the same weight" \
  " as the requested material." \
  "\n\n" \
  "The supplying of mixed material is constrained by available inventory" \
  " quantities and a per time step throughput limit.  Requests for fuel" \
  " material larger than the throughput can never be met.  Fissile inventory" \
  " can be requested/received via one or more commodities.  The DRE request" \
  " preference for each of these commodities can also optionally be specified." \
  " By default, the top-up inventory size is zero, and it is not used for" \
  " mixing. " \
  "\n\n" \
  "[1] Baker, A. R., and R. W. Ross. \"Comparison of the value of plutonium and" \
  "    uranium isotopes in fast reactors.\" Proceedings of the Conference on" \
  "    Breeding. Economics, and Safety in Large Fast Power Reactors. 1963." \
  "", \
}
 public:
  FuelFab(cyclus::Context* ctx);
  virtual ~FuelFab(){};

  virtual std::string version() { return CYCAMORE_VERSION; }

#pragma cyclus

  virtual void Tick(){};
  virtual void Tock(){};
  virtual void EnterNotify();

  virtual std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr> GetMatlBids(
      cyclus::CommodMap<cyclus::Material>::type& commod_requests);

  virtual void GetMatlTrades(
      const std::vector<cyclus::Trade<cyclus::Material> >& trades,
      std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                            cyclus::Material::Ptr> >& responses);

  virtual void AcceptMatlTrades(const std::vector<std::pair<
      cyclus::Trade<cyclus::Material>, cyclus::Material::Ptr> >& responses);

  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
  GetMatlRequests();

 private:
  #pragma cyclus var { \
    "doc": "Ordered list of commodities on which to requesting filler stream material.", \
    "uilabel": "Filler Stream Commodities", \
    "uitype": ["oneormore", "incommodity"], \
  }
  std::vector<std::string> fill_commods;
  #pragma cyclus var { \
    "default": [], \
    "uilabel": "Filler Stream Preferences", \
    "doc": "Filler stream commodity request preferences for each of the given filler commodities (same order)." \
           " If unspecified, default is to use 1.0 for all preferences.", \
  }
  std::vector<double> fill_commod_prefs;
  #pragma cyclus var { \
    "doc": "Name of recipe to be used in filler material stream requests.", \
    "uilabel": "Filler Stream Recipe", \
    "uitype": "inrecipe", \
  }
  std::string fill_recipe;
  #pragma cyclus var { \
    "doc": "Size of filler material stream inventory.", \
    "uilabel": "Filler Stream Inventory Capacity", \
    "units": "kg", \
  }
  double fill_size;
  #pragma cyclus var {"capacity": "fill_size"}
  cyclus::toolkit::ResBuf<cyclus::Material> fill;

  #pragma cyclus var { \
    "doc": "Ordered list of commodities on which to requesting fissile stream material.", \
    "uilabel": "Fissile Stream Commodities", \
    "uitype": ["oneormore", "incommodity"], \
  }
  std::vector<std::string> fiss_commods;
  #pragma cyclus var { \
    "default": [], \
    "uilabel": "Fissile Stream Preferences", \
    "doc": "Fissile stream commodity request preferences for each of the given fissile commodities (same order)." \
           " If unspecified, default is to use 1.0 for all preferences.", \
  }
  std::vector<double> fiss_commod_prefs;
  #pragma cyclus var { \
    "doc": "Name for recipe to be used in fissile stream requests." \
           " Empty string results in use of an empty dummy recipe.", \
    "uitype": "inrecipe", \
    "uilabel": "Fissile Stream Recipe", \
    "default": "", \
  }
  std::string fiss_recipe;
  #pragma cyclus var { \
    "doc": "Size of fissile material stream inventory.", \
    "uilabel": "Fissile Stream Inventory Capacity", \
    "units": "kg", \
  }
  double fiss_size;
  #pragma cyclus var {"capacity": "fiss_size"}
  cyclus::toolkit::ResBuf<cyclus::Material> fiss;

  #pragma cyclus var { \
    "doc": "Commodity on which to request material for top-up stream." \
           " This MUST be set if 'topup_size > 0'.", \
    "uilabel": "Top-up Stream Commodity", \
    "default": "", \
    "uitype": "incommodity", \
  }
  std::string topup_commod;
  #pragma cyclus var { \
    "doc": "Top-up material stream request preference.", \
    "uilabel": "Top-up Stream Preference", \
    "default": 1.0, \
  }
  double topup_pref; // default must be in range (0, cyclus::kDefaultPref)

  #pragma cyclus var { \
    "doc": "Name of recipe to be used in top-up material stream requests." \
           " This MUST be set if 'topup_size > 0'.", \
    "uilabel": "Top-up Stream Recipe", \
    "uitype": "inrecipe", \
    "default": "", \
  }
  std::string topup_recipe;
  #pragma cyclus var { \
    "doc": "Size of top-up material stream inventory.", \
    "uilabel": "Top-up Stream Inventory Capacity", \
    "units": "kg", \
    "default": 0, \
  }
  double topup_size;
  #pragma cyclus var {"capacity": "topup_size"}
  cyclus::toolkit::ResBuf<cyclus::Material> topup;

  #pragma cyclus var { \
    "doc": "Commodity on which to offer/supply mixed fuel material.", \
    "uilabel": "Output Commodity", \
    "uitype": "outcommodity", \
  }
  std::string outcommod;

  #pragma cyclus var { \
    "doc": "Maximum number of kg of fuel material that can be supplied per time step.", \
    "uilabel": "Maximum Throughput", \
    "units": "kg", \
    "default": 1e299, \
    "uitype": "range", \
    "range": [0.0, 1e299], \
  }
  double throughput;

  #pragma cyclus var {		\
    "uilabel": "Spectrum type", \
    "uitype": "combobox", \
    "categorical": ["fission_spectrum_ave", "thermal"], \
    "doc": "The type of cross-sections to use for composition property calculation." \
           " Use 'fission_spectrum_ave' for fast reactor compositions or 'thermal' for thermal reactors.", \
  }
  std::string spectrum;

  // intra-time-step state - no need to be a state var
  // map<request, inventory name>
  std::map<cyclus::Request<cyclus::Material>*, std::string> req_inventories_;

  #pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Geographical latitude in degrees as a double", \
    "doc": "Latitude of the agent's geographical position. The value should " \
           "be expressed in degrees as a double." \
  }
  double latitude;

  #pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Geographical longitude in degrees as a double", \
    "doc": "Longitude of the agent's geographical position. The value should " \
           "be expressed in degrees as a double." \
  }
  double longitude;

  cyclus::toolkit::Position coordinates;

  /// Records an agent's latitude and longitude to the output db
  void RecordPosition();
};

double CosiWeight(cyclus::Composition::Ptr c, const std::string& spectrum);
bool ValidWeights(double w_low, double w_tgt, double w_high);
double LowFrac(double w_low, double w_tgt, double w_high, double eps = 1e-6);
double HighFrac(double w_low, double w_tgt, double w_high, double eps = 1e-6);
double AtomToMassFrac(double atomfrac, cyclus::Composition::Ptr c1, cyclus::Composition::Ptr c2);

} // namespace cycamore


#endif  // CYCAMORE_SRC_FUEL_FAB_H_

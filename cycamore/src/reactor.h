#ifndef CYCAMORE_SRC_REACTOR_H_
#define CYCAMORE_SRC_REACTOR_H_

#include "cyclus.h"
#include "cycamore_version.h"

namespace cycamore {

/// Reactor is a simple, general reactor based on static compositional
/// transformations to model fuel burnup.  The user specifies a set of input
/// fuels and corresponding burnt compositions that fuel is transformed to when
/// it is discharged from the core.  No incremental transmutation takes place.
/// Rather, at the end of an operational cycle, the batch being discharged from
/// the core is instantaneously transmuted from its original fresh fuel
/// composition into its spent fuel form.
///
/// Each fuel is identified by a specific input commodity and has an associated
/// input recipe (nuclide composition), output recipe, output commidity, and
/// preference.  The preference identifies which input fuels are preferred when
/// requesting.  Changes in these preferences can be specified as a function of
/// time using the pref_change variables.  Changes in the input-output recipe
/// compositions can also be specified as a function of time using the
/// recipe_change variables.
///
/// The reactor treats fuel as individual assemblies that are never split,
/// combined or otherwise treated in any non-discrete way.  Fuel is requested
/// in full-or-nothing assembly sized quanta.  If real-world assembly modeling
/// is unnecessary, parameters can be adjusted (e.g. n_assem_core, assem_size,
/// n_assem_batch).  At the end of every cycle, a full batch is discharged from
/// the core consisting of n_assem_batch assemblies of assem_size kg. The
/// reactor also has a specifiable refueling time period following the end of
/// each cycle at the end of which it will resume operation on the next cycle
/// *if* it has enough fuel for a full core; otherwise it waits until it has
/// enough fresh fuel assemblies.
///
/// In addition to its core, the reactor has an on-hand fresh fuel inventory
/// and a spent fuel inventory whose capacities are specified by n_assem_fresh
/// and n_assem_spent respectively.  Each time step the reactor will attempt to
/// acquire enough fresh fuel to fill its fresh fuel inventory (and its core if
/// the core isn't currently full).  If the fresh fuel inventory has zero
/// capacity, fuel will be ordered just-in-time after the end of each
/// operational cycle before the next begins.  If the spent fuel inventory
/// becomes full, the reactor will halt operation at the end of the next cycle
/// until there is more room.  Each time step, the reactor will try to trade
/// away as much of its spent fuel inventory as possible.
///
/// When the reactor reaches the end of its lifetime, it will discharge all
/// material from its core and trade away all its spent fuel as quickly as
/// possible.  Full decommissioning will be delayed until all spent fuel is
/// gone.  If the reactor has a full core when it is decommissioned (i.e. is
/// mid-cycle) when the reactor is decommissioned, half (rounded up to nearest
/// int) of its assemblies are transmuted to their respective burnt
/// compositions.

class Reactor : public cyclus::Facility,
  public cyclus::toolkit::CommodityProducer,
  public cyclus::toolkit::Position {
#pragma cyclus note { \
"niche": "reactor", \
"doc": \
  "Reactor is a simple, general reactor based on static compositional" \
  " transformations to model fuel burnup.  The user specifies a set of input" \
  " fuels and corresponding burnt compositions that fuel is transformed to when" \
  " it is discharged from the core.  No incremental transmutation takes place." \
  " Rather, at the end of an operational cycle, the batch being discharged from" \
  " the core is instantaneously transmuted from its original fresh fuel" \
  " composition into its spent fuel form." \
  "\n\n" \
  "Each fuel is identified by a specific input commodity and has an associated" \
  " input recipe (nuclide composition), output recipe, output commidity, and" \
  " preference.  The preference identifies which input fuels are preferred when" \
  " requesting.  Changes in these preferences can be specified as a function of" \
  " time using the pref_change variables.  Changes in the input-output recipe" \
  " compositions can also be specified as a function of time using the" \
  " recipe_change variables." \
  "\n\n" \
  "The reactor treats fuel as individual assemblies that are never split," \
  " combined or otherwise treated in any non-discrete way.  Fuel is requested" \
  " in full-or-nothing assembly sized quanta.  If real-world assembly modeling" \
  " is unnecessary, parameters can be adjusted (e.g. n_assem_core, assem_size," \
  " n_assem_batch).  At the end of every cycle, a full batch is discharged from" \
  " the core consisting of n_assem_batch assemblies of assem_size kg. The" \
  " reactor also has a specifiable refueling time period following the end of" \
  " each cycle at the end of which it will resume operation on the next cycle" \
  " *if* it has enough fuel for a full core; otherwise it waits until it has" \
  " enough fresh fuel assemblies." \
  "\n\n" \
  "In addition to its core, the reactor has an on-hand fresh fuel inventory" \
  " and a spent fuel inventory whose capacities are specified by n_assem_fresh" \
  " and n_assem_spent respectively.  Each time step the reactor will attempt to" \
  " acquire enough fresh fuel to fill its fresh fuel inventory (and its core if" \
  " the core isn't currently full).  If the fresh fuel inventory has zero" \
  " capacity, fuel will be ordered just-in-time after the end of each" \
  " operational cycle before the next begins.  If the spent fuel inventory" \
  " becomes full, the reactor will halt operation at the end of the next cycle" \
  " until there is more room.  Each time step, the reactor will try to trade" \
  " away as much of its spent fuel inventory as possible." \
  "\n\n" \
  "When the reactor reaches the end of its lifetime, it will discharge all" \
  " material from its core and trade away all its spent fuel as quickly as" \
  " possible.  Full decommissioning will be delayed until all spent fuel is" \
  " gone.  If the reactor has a full core when it is decommissioned (i.e. is" \
  " mid-cycle) when the reactor is decommissioned, half (rounded up to nearest" \
  " int) of its assemblies are transmuted to their respective burnt" \
  " compositions." \
  "", \
}

 public:
  Reactor(cyclus::Context* ctx);
  virtual ~Reactor(){};

  virtual std::string version() { return CYCAMORE_VERSION; }

  virtual void Tick();
  virtual void Tock();
  virtual void EnterNotify();
  virtual bool CheckDecommissionCondition();

  virtual void AcceptMatlTrades(const std::vector<std::pair<
      cyclus::Trade<cyclus::Material>, cyclus::Material::Ptr> >& responses);

  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
  GetMatlRequests();

  virtual std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr> GetMatlBids(
      cyclus::CommodMap<cyclus::Material>::type& commod_requests);

  virtual void GetMatlTrades(
      const std::vector<cyclus::Trade<cyclus::Material> >& trades,
      std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                            cyclus::Material::Ptr> >& responses);

  #pragma cyclus decl

 private:
  std::string fuel_incommod(cyclus::Material::Ptr m);
  std::string fuel_outcommod(cyclus::Material::Ptr m);
  std::string fuel_inrecipe(cyclus::Material::Ptr m);
  std::string fuel_outrecipe(cyclus::Material::Ptr m);
  double fuel_pref(cyclus::Material::Ptr m);

  bool retired() {
    return exit_time() != -1 && context()->time() > exit_time();
  }

  /// Store fuel info index for the given resource received on incommod.
  void index_res(cyclus::Resource::Ptr m, std::string incommod);

  /// Discharge a batch from the core if there is room in the spent fuel
  /// inventory.  Returns true if a batch was successfully discharged.
  bool Discharge();

  /// Top up core inventory as much as possible.
  void Load();

  /// Transmute the batch that is about to be discharged from the core to its
  /// fully burnt state as defined by its outrecipe.
  void Transmute();

  /// Records production of side products from the reactor
  void RecordSideProduct(bool produce);

  /// Transmute the specified number of assemblies in the core to their
  /// fully burnt state as defined by their outrecipe.
  void Transmute(int n_assem);

  /// Records a reactor event to the output db with the given name and note val.
  void Record(std::string name, std::string val);

  /// Complement of PopSpent - must be called with all materials passed that
  /// were not traded away to other agents.
  void PushSpent(std::map<std::string, cyclus::toolkit::MatVec> leftover);

  /// Returns all spent assemblies indexed by outcommod - removing them from
  /// the spent fuel buffer.
  std::map<std::string, cyclus::toolkit::MatVec> PopSpent();

  /// Returns all spent assemblies indexed by outcommod without removing them
  /// from the spent fuel buffer.
  std::map<std::string, cyclus::toolkit::MatVec> PeekSpent();

  /////// fuel specifications /////////
  #pragma cyclus var { \
    "uitype": ["oneormore", "incommodity"], \
    "uilabel": "Fresh Fuel Commodity List", \
    "doc": "Ordered list of input commodities on which to requesting fuel.", \
  }
  std::vector<std::string> fuel_incommods;
  #pragma cyclus var { \
    "uitype": ["oneormore", "inrecipe"], \
    "uilabel": "Fresh Fuel Recipe List", \
    "doc": "Fresh fuel recipes to request for each of the given fuel input " \
           "commodities (same order).", \
  }
  std::vector<std::string> fuel_inrecipes;

  #pragma cyclus var { \
    "default": [], \
    "uilabel": "Fresh Fuel Preference List", \
    "doc": "The preference for each type of fresh fuel requested corresponding"\
           " to each input commodity (same order).  If no preferences are " \
           "specified, 1.0 is used for all fuel " \
           "requests (default).", \
  }
  std::vector<double> fuel_prefs;
  #pragma cyclus var { \
    "uitype": ["oneormore", "outcommodity"], \
    "uilabel": "Spent Fuel Commodity List", \
    "doc": "Output commodities on which to offer spent fuel originally " \
           "received as each particular input commodity (same order)." \
  }
  std::vector<std::string> fuel_outcommods;
  #pragma cyclus var {           \
    "uitype": ["oneormore", "outrecipe"], \
    "uilabel": "Spent Fuel Recipe List", \
    "doc": "Spent fuel recipes corresponding to the given fuel input " \
           "commodities (same order)." \
           " Fuel received via a particular input commodity is transmuted to " \
           "the recipe specified here after being burned during a cycle.", \
  }
  std::vector<std::string> fuel_outrecipes;

  ///////////// recipe changes ///////////
  #pragma cyclus var { \
    "default": [], \
    "uilabel": "Time to Change Fresh/Spent Fuel Recipe", \
    "doc": "A time step on which to change the input-output recipe pair for " \
           "a requested fresh fuel.", \
  }
  std::vector<int> recipe_change_times;
  #pragma cyclus var { \
    "default": [], \
    "uilabel": "Commodity for Changed Fresh/Spent Fuel Recipe", \
    "doc": "The input commodity indicating fresh fuel for which recipes will " \
           "be changed. Same order as and direct correspondence to the " \
           "specified recipe change times.", \
    "uitype": ["oneormore", "incommodity"], \
  }
  std::vector<std::string> recipe_change_commods;
  #pragma cyclus var { \
    "default": [], \
    "uilabel": "New Recipe for Fresh Fuel", \
    "doc": "The new input recipe to use for this recipe change." \
           " Same order as and direct correspondence to the specified recipe " \
           "change times.", \
    "uitype": ["oneormore", "inrecipe"], \
  }
  std::vector<std::string> recipe_change_in;
  #pragma cyclus var { \
    "default": [], \
    "uilabel": "New Recipe for Spent Fuel", \
    "doc": "The new output recipe to use for this recipe change." \
           " Same order as and direct correspondence to the specified recipe " \
           "change times.", \
    "uitype": ["oneormore", "outrecipe"], \
  }
  std::vector<std::string> recipe_change_out;

 //////////// inventory and core params ////////////
  #pragma cyclus var { \
    "doc": "Mass (kg) of a single assembly.", \
    "uilabel": "Assembly Mass", \
    "uitype": "range", \
    "range": [1.0, 1e5], \
    "units": "kg", \
  }
  double assem_size;

  #pragma cyclus var { \
    "uilabel": "Number of Assemblies per Batch", \
    "doc": "Number of assemblies that constitute a single batch.  " \
           "This is the number of assemblies discharged from the core fully " \
           "burned each cycle."           \
           "Batch size is equivalent to ``n_assem_batch / n_assem_core``.", \
  }
  int n_assem_batch;
  #pragma cyclus var { \
    "default": 3, \
    "uilabel": "Number of Assemblies in Core", \
    "uitype": "range", \
    "range": [1,3], \
    "doc": "Number of assemblies that constitute a full core.", \
  }
  int n_assem_core;
  #pragma cyclus var { \
    "default": 0, \
    "uilabel": "Minimum Fresh Fuel Inventory", \
    "uitype": "range", \
    "range": [0,3], \
    "units": "assemblies", \
    "doc": "Number of fresh fuel assemblies to keep on-hand if possible.", \
  }
  int n_assem_fresh;
  #pragma cyclus var { \
    "default": 1000000000, \
    "uilabel": "Maximum Spent Fuel Inventory", \
    "uitype": "range", \
    "range": [0, 1000000000], \
    "units": "assemblies", \
    "doc": "Number of spent fuel assemblies that can be stored on-site before" \
           " reactor operation stalls.", \
  }
  int n_assem_spent;

   ///////// cycle params ///////////
  #pragma cyclus var { \
    "default": 18, \
    "doc": "The duration of a full operational cycle (excluding refueling " \
           "time) in time steps.", \
    "uilabel": "Cycle Length", \
    "units": "time steps", \
  }
  int cycle_time;
  #pragma cyclus var { \
    "default": 1, \
    "doc": "The duration of a full refueling period - the minimum time between"\
           " the end of a cycle and the start of the next cycle.", \
    "uilabel": "Refueling Outage Duration", \
    "units": "time steps", \
  }
  int refuel_time;
  #pragma cyclus var { \
    "default": 0, \
    "doc": "Number of time steps since the start of the last cycle." \
           " Only set this if you know what you are doing", \
    "uilabel": "Time Since Start of Last Cycle", \
    "units": "time steps", \
  }
  int cycle_step;

  //////////// power params ////////////
  #pragma cyclus var { \
    "default": 0, \
    "doc": "Amount of electrical power the facility produces when operating " \
           "normally.", \
    "uilabel": "Nominal Reactor Power", \
    "uitype": "range", \
    "range": [0.0, 2000.00],  \
    "units": "MWe", \
  }
  double power_cap;

  #pragma cyclus var { \
    "default": "power", \
    "uilabel": "Power Commodity Name", \
    "doc": "The name of the 'power' commodity used in conjunction with a " \
           "deployment curve.", \
  }
  std::string power_name;

  /////////// hybrid params ///////////

  #pragma cyclus var { \
    "uilabel": "Side Product from Reactor Plant", \
    "default": [], \
    "doc": "Ordered vector of side product the reactor produces with power", \
  }
  std::vector<std::string> side_products;

  #pragma cyclus var { \
    "uilabel": "Quantity of Side Product from Reactor Plant", \
    "default": [], \
    "doc": "Ordered vector of the quantity of side product the reactor produces with power", \
  }
  std::vector<double> side_product_quantity;

  #pragma cyclus var {"default": 1,\
                      "internal": True,\
                      "doc": "True if reactor is a hybrid system (produces side products)", \
  }
  bool hybrid_;


  /////////// Decommission transmutation behavior ///////////
  #pragma cyclus var {"default": 0, \
                      "uilabel": "Boolean for transmutation behavior upon decommissioning.", \
                      "doc": "If true, the archetype transmutes all assemblies upon decommissioning " \
                             "If false, the archetype only transmutes half.", \
  }
  bool decom_transmute_all;


  /////////// preference changes ///////////
  #pragma cyclus var { \
    "default": [], \
    "uilabel": "Time to Change Fresh Fuel Preference", \
    "doc": "A time step on which to change the request preference for a " \
           "particular fresh fuel type.", \
  }
  std::vector<int> pref_change_times;
  #pragma cyclus var { \
    "default": [], \
    "doc": "The input commodity for a particular fuel preference change.  " \
           "Same order as and direct correspondence to the specified " \
           "preference change times.", \
    "uilabel": "Commodity for Changed Fresh Fuel Preference", \
    "uitype": ["oneormore", "incommodity"], \
  }
  std::vector<std::string> pref_change_commods;
  #pragma cyclus var { \
    "default": [], \
    "uilabel": "Changed Fresh Fuel Preference",                        \
    "doc": "The new/changed request preference for a particular fresh fuel." \
           " Same order as and direct correspondence to the specified " \
           "preference change times.", \
  }
  std::vector<double> pref_change_values;

  // Resource inventories - these must be defined AFTER/BELOW the member vars
  // referenced (e.g. n_batch_fresh, assem_size, etc.).
  #pragma cyclus var {"capacity": "n_assem_fresh * assem_size"}
  cyclus::toolkit::ResBuf<cyclus::Material> fresh;
  #pragma cyclus var {"capacity": "n_assem_core * assem_size"}
  cyclus::toolkit::ResBuf<cyclus::Material> core;
  #pragma cyclus var {"capacity": "n_assem_spent * assem_size"}
  cyclus::toolkit::ResBuf<cyclus::Material> spent;


  // should be hidden in ui (internal only). True if fuel has already been
  // discharged this cycle.
  #pragma cyclus var {"default": 0, "doc": "This should NEVER be set manually",\
                      "internal": True \
  }
  bool discharged;

  // This variable should be hidden/unavailable in ui.  Maps resource object
  // id's to the index for the incommod through which they were received.
  #pragma cyclus var {"default": {}, "doc": "This should NEVER be set manually", \
                      "internal": True \
  }
  std::map<int, int> res_indexes;

  // populated lazily and no need to persist.
  std::set<std::string> uniq_outcommods_;

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

} // namespace cycamore

#endif  // CYCAMORE_SRC_REACTOR_H_

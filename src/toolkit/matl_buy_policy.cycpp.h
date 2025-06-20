/// This includes the required header to add a material buy policy to archetypes.
/// One should only need to:
/// - '#include "toolkit/matl_buy_policy.cycpp.h"' in the header of the
///    archetype class (as private)
/// - Add `InitBuyPolicyParameters()` to `EnterNotify()` in the cc file of the
///   archetype class as needed.

/// How to add parameters to this file:
/// 1. Add the pragma. A default value MUST be added to ensure backwards
///    compatibility.
/// 2. Add "std::vector<int> cycpp_shape_<param_name> = {0};" to the end of the
///    file with the other ones, reaplcing <param_name> with the name you put
///    in the econ_params array (again, must match exactly).



/// @addtogroup optionalparams
/// @{
/// active_buying_frequency_type is the type of distribution used to determine the length of the active buying period
/// active_buying_val is the length of the active buying period if active_buying_frequency_type is Fixed
/// active_buying_min is the minimum length of the active buying period if active_buying_frequency_type is Uniform (required) or 
/// Normal (optional)
/// active_buying_max is the maximum length of the active buying period if active_buying_frequency_type is Uniform (required) or
/// Normal (optional)
/// active_buying_mean is the mean length of the active buying period if active_buying_frequency_type is Normal
/// active_buying_stddev is the standard deviation of the active buying period if active_buying_frequency_type is Normal
/// active_buying_end_probability is the probability that at any given timestep, the agent ends the active buying 
///                               period if the active buying frequency type is Binomial
/// active_buying_disruption_probability is the probability that in any given cycle, the agent undergoes a disruption 
///                               (disrupted active period) if the active buying frequency type is FixedWithDisruption
/// active_buying_disruption is the length of the disrupted active cycle if the active buying frequency type is 
///                               FixedWithDisruption
/// dormant_buying_frequency_type is the type of distribution used to determine the length of the dormant buying period
/// dormant_buying_val is the length of the dormant buying period if dormant_buying_frequency_type is Fixed
/// dormant_buying_min is the minimum length of the dormant buying period if dormant_buying_frequency_type is Uniform (required) or
/// Normal (optional)
/// dormant_buying_max is the maximum length of the dormant buying period if dormant_buying_frequency_type is Uniform (required) or
/// Normal (optional)
/// dormant_buying_mean is the mean length of the dormant buying period if dormant_buying_frequency_type is Normal
/// dormant_buying_stddev is the standard deviation of the dormant buying period if dormant_buying_frequency_type is Normal
/// dormant_buying_end_probability is the probability that at any given timestep, the agent ends the dormant buying period if
///                               the dormant buying frequency type is Binomial
/// dormant_buying_disruption_probability is the probability that in any given cycle, the agent undergoes a disruption (disrupted
///                               offline period) if the dormant buying frequency type is FixedWithDisruption
/// dormant_buying_disruption is the length of the disrupted dormant cycle if the dormant buying frequency type is 
///                               FixedWithDisruption
/// buying_size_type is the type of distribution used to determine the size of buy requests, as a fraction of the current capacity
/// buying_size_val is the size of the buy request for Fixed  buying_size_type
/// buying_size_min is the minimum size of the buy request if buying_size_type is Uniform (required) or Normal (optional)
/// buying_size_max is the maximum size of the buy request if buying_size_type is Uniform (required) or Normal (optional)
/// buying_size_mean is the mean size of the buy request if buying_size_type is Normal
/// buying_size_stddev is the standard deviation of the buy request if buying_size_type is Normal
/// @}



///   @brief sets up the distributions for the buy policy
inline void InitBuyPolicyParameters() {
  /// set up active buying distribution
  if (active_buying_min > active_buying_max) {
    throw cyclus::ValueError("Active min larger than max.");
  }
  if (dormant_buying_min > dormant_buying_max) {
    throw cyclus::ValueError("Dormant min larger than max.");
  }
  if (buying_size_min > buying_size_max) {
    throw cyclus::ValueError("Buying size min larger than max.");
  }

  if (active_buying_frequency_type == "Fixed") {
    active_dist_ = cyclus::FixedIntDist::Ptr (new cyclus::FixedIntDist(active_buying_val));
  }
  else if (active_buying_frequency_type == "Uniform") {
    if ((active_buying_min == -1) || (active_buying_max == -1)) {
      throw cyclus::ValueError("Invalid active buying frequency range. Please provide both a min and max value.");
    }
    active_dist_ = cyclus::UniformIntDist::Ptr (new cyclus::UniformIntDist(active_buying_min, active_buying_max));
  }
  else if (active_buying_frequency_type == "Normal") {
    if ((active_buying_mean == -1) || (active_buying_stddev == -1)) {
      throw cyclus::ValueError("Invalid active buying frequency range. Please provide both a mean and standard deviation value.");
    }
    if (active_buying_min == -1) {active_buying_min = 1;}
    if (active_buying_max == -1) {
      active_buying_max = std::numeric_limits<int>::max();}

    active_dist_ = cyclus::NormalIntDist::Ptr (new cyclus::NormalIntDist(active_buying_mean, active_buying_stddev,
                          active_buying_min, active_buying_max));
  }
  else if (active_buying_frequency_type == "Binomial") {
    if (active_buying_end_probability < 0 || active_buying_end_probability > 1) {
      throw cyclus::ValueError("Active buying end probability must be between 0 and 1");
    }
    int success = 1; // only one success is needed to end the active buying period
    active_dist_ = cyclus::NegativeBinomialIntDist::Ptr (new cyclus::NegativeBinomialIntDist(success, active_buying_end_probability));
  } else if (active_buying_frequency_type == "FixedWithDisruption") {
    if (active_buying_disruption < 0) {
      throw cyclus::ValueError("Disruption must be greater than or equal to 0");
    }
    active_dist_ = cyclus::BinaryIntDist::Ptr (
      new cyclus::BinaryIntDist(active_buying_disruption_probability,
      active_buying_disruption, active_buying_val));
  }
  else {
    throw cyclus::ValueError("Invalid active buying frequency type");}

  /// set up dormant buying distribution
  if (dormant_buying_frequency_type == "Fixed") {
    dormant_dist_ = cyclus::FixedIntDist::Ptr (new cyclus::FixedIntDist(dormant_buying_val));
  }
  else if (dormant_buying_frequency_type == "Uniform") {
    if ((dormant_buying_min == -1) || (dormant_buying_max == -1)) {
      throw cyclus::ValueError("Invalid dormant buying frequency range. Please provide both a min and max value.");
    }
    dormant_dist_ = cyclus::UniformIntDist::Ptr (new cyclus::UniformIntDist(dormant_buying_min, dormant_buying_max));
  }
  else if (dormant_buying_frequency_type == "Normal") {
    if ((dormant_buying_mean == -1) || (dormant_buying_stddev == -1)) {
      throw cyclus::ValueError("Invalid dormant buying frequency range. Please provide both a mean and standard deviation value.");
    }
    if (dormant_buying_min == -1) {dormant_buying_min = 1;}
    if (dormant_buying_max == -1) {
      dormant_buying_max = std::numeric_limits<int>::max();}
    dormant_dist_ = cyclus::NormalIntDist::Ptr (new cyclus::NormalIntDist(dormant_buying_mean, dormant_buying_stddev,
                          dormant_buying_min, dormant_buying_max));
  }
  else if (dormant_buying_frequency_type == "Binomial") {
    if (dormant_buying_end_probability < 0 || dormant_buying_end_probability > 1) {
      throw cyclus::ValueError("Dormant buying end probability must be between 0 and 1");
    }
    int success = 1; // only one success is needed to end the dormant buying period
    dormant_dist_ = cyclus::NegativeBinomialIntDist::Ptr (new cyclus::NegativeBinomialIntDist(success, dormant_buying_end_probability));
  } else if (dormant_buying_frequency_type == "FixedWithDisruption") {
    if (dormant_buying_disruption < 0) {
      throw cyclus::ValueError("Disruption must be greater than or equal to 0");
    }
    dormant_dist_ = cyclus::BinaryIntDist::Ptr (
      new cyclus::BinaryIntDist(dormant_buying_disruption_probability,
      dormant_buying_disruption, dormant_buying_val));
  }
  else {
    throw cyclus::ValueError("Invalid dormant buying frequency type");}

  /// set up buying size distribution
  if (buying_size_type == "Fixed") {
    size_dist_ = cyclus::FixedDoubleDist::Ptr (new cyclus::FixedDoubleDist(buying_size_val));
  }
  else if (buying_size_type == "Uniform") {
    if ((buying_size_min == -1) || (buying_size_max == -1)) {
      throw cyclus::ValueError("Invalid buying size range. Please provide both a min and max value.");
    }
    size_dist_ = cyclus::UniformDoubleDist::Ptr (new cyclus::UniformDoubleDist(buying_size_min, buying_size_max));
  }
  else if (buying_size_type == "Normal") {
    if ((buying_size_mean == -1) || (buying_size_stddev == -1)) {
      throw cyclus::ValueError("Invalid buying size range. Please provide both a mean and standard deviation value.");
    }
    if (buying_size_min == -1) {buying_size_min = 0;}
    if (buying_size_max == -1) {buying_size_max = 1;}
    size_dist_ = cyclus::NormalDoubleDist::Ptr (new cyclus::NormalDoubleDist(buying_size_mean, buying_size_stddev,
                             buying_size_min, buying_size_max));
  }
  else {
    throw cyclus::ValueError("Invalid buying size type");}
}

#pragma cyclus var {"default": "Fixed",\
                    "tooltip": "Type of active buying frequency",\
                    "doc": "Options: Fixed, Uniform, Normal, Binomial, FixedWithDisruption. "\
                    "Fixed requires active_buying_val. Uniform "\
                    "requires active_buying_min and active_buying_max.  Normal "\
                    "requires active_buying_mean and active_buying_std, with optional "\
                    "active_buying_min and active_buying_max. Binomial requires active_buying_end_probability."\
                    "FixedWithDisruption has a probability that any given cycle will have a disrupted, "\
                    "active length.  Once per cycle, a Bernoulli distribution (Binomial dist "\
                    "with N=1) will be sampled to determine if typical or disrupted cycle. If typical, "\
                    "active_buying_val is cycle length. If disrupted, active_buying_disruption.",\
                    "uitype": "combobox",\
                    "categorical": ["Fixed", "Uniform", "Normal", "Binomial", "FixedWithDisruption"],\
                    "uilabel": "Active Buying Frequency Type"}
std::string active_buying_frequency_type;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Fixed active buying frequency",\
                    "doc": "The length in time steps of the active buying period. Required for fixed "\
                    "active_buying_frequency_type. Must be greater than or equal to 1 (i.e., agent "\
                    "cannot always be dormant)",\
                    "uitype": "range", \
                    "range": [1, CY_LARGE_INT], \
                    "uilabel": "Active Buying Frequency Value"}
int active_buying_val;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Active buying distribution minimum",\
                    "doc": "The minimum length in time steps of the active buying period. Required for "\
                    "Uniform and optional for Normal active_buying_frequency_type. Must be greater than "\
                    "or equal to 1 ",\
                    "uitype": "range", \
                    "range": [1, CY_LARGE_INT], \
                    "uilabel": "Active Buying Frequency Minimum"}
int active_buying_min;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Active buying distribution maximum",\
                    "doc": "The maximum length in time steps of the active buying period. Required for "\
                    "Uniform active_buying_frequency_type, optional for Normal. Must be greater than or equal to active_buying_min ",\
                    "uitype": "range", \
                    "range": [1, CY_LARGE_INT], \
                    "uilabel": "Active Buying Frequency Maximum"}
int active_buying_max;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Active buying distribution mean",\
                    "doc": "The mean length in time steps of the active buying period. Required for "\
                    "Normal active_buying_frequency_type. Must be greater than or equal to 1 ",\
                    "uitype": "range", \
                    "range": [1.0, CY_LARGE_DOUBLE], \
                    "uilabel": "Active Buying Frequency Mean"}
double active_buying_mean;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Active buying distribution standard deviation",\
                    "doc": "The standard deviation of the length in time steps of the active buying period. "\
                    "Required for Normal active_buying_frequency_type. Must be greater than or equal to 0 ",\
                    "uitype": "range", \
                    "range": [0.0, CY_LARGE_DOUBLE], \
                    "uilabel": "Active Buying Frequency Standard Deviation"}
double active_buying_stddev;

#pragma cyclus var {"default": 0,\
                    "tooltip": "Probability that agent will go offline during the next time step",\
                    "doc": "Binomial distribution has a fixed probability of going dormant at any given "\
                    "timestep, like a weighted coin flip. Required for Binomial active_buying_frequency_type. "\
                    "Must be between 0 and 1",\
                    "uitype": "range", \
                    "range": [0.0, 1.0], \
                    "uilabel": "Active Buying Offline Probability"}
double active_buying_end_probability;

#pragma cyclus var {"default": 0,\
                    "tooltip": "Probability that a cycle contains a disruption",\
                    "doc": "Probability that the agent undergoes a disruption (disrupted active period) "\
                    "during any given cycle. Required for FixedWithDisruption active_buying_frequency_type.",\
                    "uitype": "range",\
                    "range": [0.0, 1.0],\
                    "uilabel": "Active Buying Disruption Probability"}
double active_buying_disruption_probability;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Fixed length of disrupted active cycle",\
                    "doc": "When a active cycle is disrupted, this is length of the active period instead "\
                    "of active_buying_val. Required for FixedWithDisruption active_buying_frequency_type",\
                    "uitype": "range",\
                    "range": [0, CY_LARGE_INT]}
int active_buying_disruption;

#pragma cyclus var {"default": "Fixed",\
                    "tooltip": "Type of dormant buying frequency",\
                    "doc": "Options: Fixed, Uniform, Normal, Binomial, FixedWithDisruption. "\
                    "Fixed requires dormant_buying_val. "\
                    "Uniform requires dormant_buying_min and dormant_buying_max. Normal requires "\
                    "dormant_buying_mean and dormant_buying_std, with optional dormant_buying_min "\
                    "and dormant_buying_max. Binomial requires dormant_buying_end_probability. "\
                    "FixedWithDisruption has a probability that any given cycle will have a disrupted, "\
                    "or long, outage.  Once per cycle, a Bernoulli distribution (Binomial dist "\
                    "with N=1) will be sampled to determine if typical or disrupted cycle. If typical, "\
                    "dormant_buying_val is cycle length. If disrupted, dormant_buying_disruption.",\
                    "uitype": "combobox",\
                    "categorical": ["Fixed", "Uniform", "Normal", "Binomial", "FixedWithDisruption"],\
                    "uilabel": "Dormant Buying Frequency Type"}
std::string dormant_buying_frequency_type;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Fixed dormant buying frequency",\
                    "doc": "The length in time steps of the dormant buying period. Required for fixed "\
                    "dormant_buying_frequency_type. Default is -1, agent has no dormant period and stays active.",\
                    "uitype": "range", \
                    "range": [-1, CY_LARGE_INT], \
                    "uilabel": "Dormant Buying Frequency Value"}
int dormant_buying_val;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Dormant buying distribution minimum",\
                    "doc": "The minimum length in time steps of the dormant buying period. Required for Uniform and optional for "\
                    "Normal dormant_buying_frequency_type.",\
                    "uitype": "range", \
                    "range": [0, CY_LARGE_INT], \
                    "uilabel": "Dormant Buying Frequency Minimum"}
int dormant_buying_min;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Dormant buying distribution maximum",\
                    "doc": "The maximum length in time steps of the dormant buying period. Required for "\
                    "Uniform dormant_buying_frequency_type, optional for Normal. Must be greater than or equal to dormant_buying_min ",\
                    "uitype": "range", \
                    "range": [0, CY_LARGE_INT], \
                    "uilabel": "Dormant Buying Frequency Maximum"}
int dormant_buying_max;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Dormant buying distribution mean",\
                    "doc": "The mean length in time steps of the dormant buying period. Required for "\
                    "Normal dormant_buying_frequency_type. Must be greater than or equal to 0 ",\
                    "uitype": "range", \
                    "range": [0.0, CY_LARGE_DOUBLE], \
                    "uilabel": "Dormant Buying Frequency Mean"}
double dormant_buying_mean;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Dormant buying distribution standard deviation",\
                    "doc": "The standard deviation of the length in time steps of the dormant buying period. "\
                    "Required for Normal dormant_buying_frequency_type. Must be greater than or equal to 0 ",\
                    "uitype": "range", \
                    "range": [0.0, CY_LARGE_DOUBLE], \
                    "uilabel": "Dormant Buying Frequency Standard Deviation"}
double dormant_buying_stddev;

#pragma cyclus var {"default": 0,\
                    "tooltip": "Probability that agent will return to active during the next time step",\
                    "doc": "Binomial distribution has a fixed probability of going active at any given "\
                    "timestep, like a weighted coin flip. Required for Binomial dormant_buying_frequency_type. "\
                    "Must be between 0 and 1",\
                    "uitype": "range", \
                    "range": [0.0, 1.0], \
                    "uilabel": "Dormant Buying Binomial Offline Probability"}
double dormant_buying_end_probability;

#pragma cyclus var {"default": 0,\
                    "tooltip": "Probability that a cycle contains a disruption",\
                    "doc": "Probability that the agent undergoes a disruption (longer offline period) "\
                    "during any given cycle. Required for FixedWithDisruption dormant_buying_frequency_type.",\
                    "uitype": "range",\
                    "range": [0.0, 1.0],\
                    "uilabel": "Dormant Buying Disruption Probability"}
double dormant_buying_disruption_probability;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Fixed length of disrupted cycle",\
                    "doc": "When a dormant cycle is disrupted, this is length of the offline period instead "\
                    "of dormant_buying_val. Required for FixedWithDisruption dormant_buying_frequency_type",\
                    "uitype": "range",\
                    "range": [0, CY_LARGE_INT]}
int dormant_buying_disruption;

#pragma cyclus var {"default": "Fixed",\
                    "tooltip": "Type of behavior used to determine size of buy request",\
                    "doc": "Behavior function used to determine the size of requests made. All values are "\
                    "a fraction of maximum capacity, determined by the throughput and capacity remaining."\
                    " Options: Fixed, Uniform, Normal. Fixed is default behavior. Uniform requires "\
                    "buying_size_min and buying_size_max. Normal requires "\
                    "buying_size_mean and buying_size_stddev, optional buying_size_min and "\
                    "buying_size_max.",\
                    "uitype": "combobox",\
                    "categorical": ["Fixed", "Uniform", "Normal"],\
                    "uilabel": "Buying Size Type"}
std::string buying_size_type;

#pragma cyclus var {"default": 1.0,\
                    "tooltip": "Fixed buying size",\
                    "doc": "The size of the buy request as a fraction of maximum capacity. Optional for Fixed "\
                    "buying_size_type. Must be greater than or equal to 0.0",\
                    "uitype": "range", \
                    "range": [0.0, 1.0], \
                    "uilabel": "Buying Size Value"}
double buying_size_val;

#pragma cyclus var {"default": -1.0,\
                    "tooltip": "Buying size distribution minimum",\
                    "doc": "The minimum size of the buy request as a fraction of maximum capacity. "\
                    "Required for Uniform and optional for Normal buying_size_type. Must be greater than "\
                    "or equal to zero.",\
                    "uitype": "range", \
                    "range": [0.0, 1.0], \
                    "uilabel": "Buying Size Minimum"}
double buying_size_min;

#pragma cyclus var {"default": -1.0,\
                    "tooltip": "Buying size distribution maximum",\
                    "doc": "The maximum size of the buy request as a fraction of maximum capacity. "\
                    "Required for Uniform buying_size_type, optional for Normal. Must be greater than "\
                    "or equal to buying_size_min ",\
                    "uitype": "range", \
                    "range": [0.0, 1.0], \
                    "uilabel": "Buying Size Maximum"}
double buying_size_max;

#pragma cyclus var {"default": -1.0,\
                    "tooltip": "Buying size distribution mean",\
                    "doc": "The mean size of the buy request as a fraction of maximum capacity. "\
                    "Required for Normal buying_size_type.",\
                    "uitype": "range", \
                    "range": [0.0, 1.0], \
                    "uilabel": "Buying Size Mean"}
double buying_size_mean;

#pragma cyclus var {"default": -1.0,\
                    "tooltip": "Buying size distribution standard deviation",\
                    "doc": "The standard deviation of the size of the buy request as a fraction of "\
                    "maximum capacity. Required for Normal buying_size_type.",\
                    "uitype": "range", \
                    "range": [0.0, 1.0], \
                    "uilabel": "Buying Size Standard Deviation"}
double buying_size_stddev;

#pragma cyclus var {"default": -1,\
                    "tooltip":"Reorder point",\
                    "doc":"The point at which the facility will request more material. "\
                    "Above this point, no request will be made. Must be less than max_inv_size."\
                    "If paired with reorder_quantity, this agent will have an (R,Q) inventory policy. "\
                    "If reorder_point is used alone, this agent will have an (s,S) inventory policy, "\
                    " with S (the maximum) being set at max_inv_size.",\
                    "uilabel":"Reorder Point"}
double reorder_point;

#pragma cyclus var {"default": -1,\
                    "tooltip":"Reorder amount (R,Q inventory policy)",\
                    "doc":"The amount of material that will be requested when the reorder point is reached. "\
                    "Exclusive request, so will demand exactly reorder_quantity."\
                    "Reorder_point + reorder_quantity must be less than max_inv_size.",\
                    "uilabel":"Reorder Quantity"}
double reorder_quantity;

#pragma cyclus var {"default": -1,\
                    "tooltip": "Total amount of material that can be recieved per cycle.",\
                    "doc": "After receiving this much material cumulatively, the agent will go dormant. "\
                    "Must be paired with dormant_buying_frequency_type and any other dormant parameters. "\
                    "The per-time step demand is unchanged except the cycle cap is almost reached.",\
                    "uilabel": "Cumulative Cap"}
double cumulative_cap;

//// A policy for requesting material
cyclus::toolkit::MatlBuyPolicy buy_policy;

cyclus::IntDistribution::Ptr active_dist_ = NULL;
cyclus::IntDistribution::Ptr dormant_dist_ = NULL;
cyclus::DoubleDistribution::Ptr size_dist_ = NULL;


// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_active_buying_frequency_type = {0};
std::vector<int> cycpp_shape_active_buying_val = {0};
std::vector<int> cycpp_shape_active_buying_min = {0};
std::vector<int> cycpp_shape_active_buying_max= {0};
std::vector<int> cycpp_shape_active_buying_mean = {0};
std::vector<int> cycpp_shape_active_buying_stddev = {0};
std::vector<int> cycpp_shape_active_buying_end_probability = {0};
std::vector<int> cycpp_shape_active_buying_disruption_probability = {0};
std::vector<int> cycpp_shape_active_buying_disruption = {0};
std::vector<int> cycpp_shape_dormant_buying_frequency_type = {0};
std::vector<int> cycpp_shape_dormant_buying_val = {0};
std::vector<int> cycpp_shape_dormant_buying_min = {0};
std::vector<int> cycpp_shape_dormant_buying_max = {0};
std::vector<int> cycpp_shape_dormant_buying_mean = {0};
std::vector<int> cycpp_shape_dormant_buying_stddev = {0};
std::vector<int> cycpp_shape_dormant_buying_end_probability = {0};
std::vector<int> cycpp_shape_dormant_buying_disruption_probability = {0};
std::vector<int> cycpp_shape_dormant_buying_disruption = {0};
std::vector<int> cycpp_shape_buying_size_type = {0};
std::vector<int> cycpp_shape_buying_size_val = {0};
std::vector<int> cycpp_shape_buying_size_min = {0};
std::vector<int> cycpp_shape_buying_size_max = {0};
std::vector<int> cycpp_shape_buying_size_mean = {0};
std::vector<int> cycpp_shape_buying_size_stddev = {0};
std::vector<int> cycpp_shape_reorder_point = {0};
std::vector<int> cycpp_shape_reorder_quantity = {0};
std::vector<int> cycpp_shape_cumulative_cap = {0};
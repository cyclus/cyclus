/// This includes the required header to add marginal utility functional forms
/// to archetypes.
/// One should only need to:
/// - '#include "toolkit/marginal_utility.cycpp.h"' in the header of the
///    archetype class (as private)
/// - The functional form and parameters will be available as state variables
///    that can be set in the input file.
/// - Call `InitializeMarginalUtility(commods, prefs)` in `EnterNotify()` or
///    `Build()` to calculate and cache marginal utilities for all commodities
/// - Use `GetMarginalUtility(commodity_name)` when constructing bids to
///    retrieve the cached marginal utility value for a specific commodity

/// How to add parameters to this file:
/// 1. Add the pragma. A default value MUST be added to ensure backwards
///    compatibility.
/// 2. Add "std::vector<int> cycpp_shape_<param_name> = {0};" to the end of the
///    file with the other ones, replacing <param_name> with the name you put
///    in the pragma (must match exactly).

// clang-format off
#pragma cyclus var { \
    "default": "Linear", \
    "uilabel": "Marginal Utility Functional Form. Must be one of: Linear, Affine, Exponential, Logarithmic", \
    "uitype": "combobox", \
    "categorical": ["Linear", "Affine", "Exponential", "Logarithmic"], \
    "uitype": "range", \
    "range": [0.0, CY_LARGE_DOUBLE], \
    "doc": "The functional form to use for calculating marginal utility. Must be one of: Linear, Affine, Exponential, Logarithmic" \
    }
std::string mu_functional_form;

#pragma cyclus var { \
    "default": [1.0], \
    "uilabel": "Marginal Utility Parameters", \
    "doc": "Parameters for the marginal utility functional form. The number and meaning of parameters depends on the selected functional form." \
    }
std::vector<double> mu_parameters;
// clang-format on

// Cache for marginal utility values (commodity name -> MU value)
std::map<std::string, double> mu_cache_;

/// @brief Calculates the marginal utility based on the selected functional form
/// and parameters.
///
/// Functional forms:
/// - Linear: MU = k * P (requires 1 parameter: k)
/// - Affine: MU = b + k * P (requires 2 parameters: b, k)
/// - Exponential: MU = B * exp(k * P) (requires 2 parameters: B, k)
/// - Logarithmic: MU = B * ln(1 + k * P) (requires 2 parameters: B, k)
///
/// @param commods Vector of commodity names
/// @param prefs Vector of preference values (must be same length as commods)
/// @return Pair of vectors: first vector contains commodity names, second vector contains marginal utility values (in matching order)
std::pair<std::vector<std::string>, std::vector<double>> CalcMarginalUtility(
    std::vector<std::string> commods, std::vector<double> prefs) const {
  std::vector<std::string> result_commods;
  std::vector<double> result_values;

  // Ensure inputs are the same length
  if (commods.size() != prefs.size()) {
    return std::make_pair(result_commods, result_values);  // Return empty vectors if sizes don't match
  }

  // Extract parameters based on functional form
  if (mu_functional_form == "Linear") {
    // Linear: MU = k * P
    double k = mu_parameters.size() > 0 ? mu_parameters[0] : 1.0;
    for (size_t i = 0; i < commods.size(); ++i) {
      double mu = k * prefs[i];
      result_commods.push_back(commods[i]);
      result_values.push_back(mu);
    }
  } else if (mu_functional_form == "Affine") {
    // Affine: MU = b + k * P
    double b = mu_parameters.size() > 0 ? mu_parameters[0] : 0.0;
    double k = mu_parameters.size() > 1 ? mu_parameters[1] : 1.0;
    for (size_t i = 0; i < commods.size(); ++i) {
      double mu = b + k * prefs[i];
      result_commods.push_back(commods[i]);
      result_values.push_back(mu);
    }
  } else if (mu_functional_form == "Exponential") {
    // Exponential: MU = B * exp(k * P)
    double B = mu_parameters.size() > 0 ? mu_parameters[0] : 1.0;
    double k = mu_parameters.size() > 1 ? mu_parameters[1] : 1.0;
    for (size_t i = 0; i < commods.size(); ++i) {
      double mu = B * std::exp(k * prefs[i]);
      result_commods.push_back(commods[i]);
      result_values.push_back(mu);
    }
  } else if (mu_functional_form == "Logarithmic") {
    // Logarithmic: MU = B * ln(1 + k * P)
    double B = mu_parameters.size() > 0 ? mu_parameters[0] : 1.0;
    double k = mu_parameters.size() > 1 ? mu_parameters[1] : 1.0;
    for (size_t i = 0; i < commods.size(); ++i) {
      double mu = B * std::log(1.0 + k * prefs[i]);
      result_commods.push_back(commods[i]);
      result_values.push_back(mu);
    }
  }

  return std::make_pair(result_commods, result_values);
}

/// @brief Initializes and caches marginal utility values for all commodities.
///
/// This function should be called once at the beginning of the simulation
/// (e.g., in EnterNotify() or Build()) to calculate and cache all marginal
/// utility values. After initialization, use GetMarginalUtility() to retrieve
/// cached values when constructing bids.
///
/// @param commods Vector of commodity names
/// @param prefs Vector of preference values (must be same length as commods)
void InitializeMarginalUtility(std::vector<std::string> commods,
                               std::vector<double> prefs) {
  mu_cache_.clear();
  auto mu_results = CalcMarginalUtility(commods, prefs);
  const auto& mu_commods = mu_results.first;
  const auto& mu_values = mu_results.second;
  for (size_t i = 0; i < mu_commods.size(); ++i) {
    mu_cache_[mu_commods[i]] = mu_values[i];
  }
}

/// @brief Retrieves the cached marginal utility value for a specific commodity.
///
/// This function returns the marginal utility value that was previously
/// calculated and cached by InitializeMarginalUtility(). Use this function
/// when constructing bids to get the MU for a specific commodity.
///
/// @param commodity_name The name of the commodity to look up
/// @return The marginal utility value for the commodity, or 0.0 if not found
double GetMarginalUtility(const std::string& commodity_name) const {
  auto it = mu_cache_.find(commodity_name);
  if (it != mu_cache_.end()) {
    return it->second;
  }
  return 0.0;  // Return 0.0 if commodity not found in cache
}

// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_mu_functional_form = {0};
std::vector<int> cycpp_shape_mu_parameters = {0};


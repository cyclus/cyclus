/// This includes the required header to add marginal utility functional forms
/// to archetypes.
/// One should only need to:
/// - '#include "toolkit/marginal_utility.cycpp.h"' in the header of the
///    archetype class (as private)
/// - The functional form and parameters will be available as state variables
///    that can be set in the input file.

/// How to add parameters to this file:
/// 1. Add the pragma. A default value MUST be added to ensure backwards
///    compatibility.
/// 2. Add "std::vector<int> cycpp_shape_<param_name> = {0};" to the end of the
///    file with the other ones, replacing <param_name> with the name you put
///    in the pragma (must match exactly).

// clang-format off
#pragma cyclus var { \
    "default": "Linear", \
    "uilabel": "Marginal Utility Functional Form. Must be one of: Linear, Affine, Exponential, Logarithmic, Piecewise", \
    "uitype": "combobox", \
    "categorical": ["Linear", "Affine", "Exponential", "Logarithmic", "Piecewise"], \
    "doc": "The functional form to use for calculating marginal utility. Must be one of: Linear, Affine, Exponential, Logarithmic, Piecewise" \
    }
std::string mu_functional_form;

#pragma cyclus var { \
    "default": [1.0], \
    "uilabel": "Marginal Utility Parameters", \
    "doc": "Parameters for the marginal utility functional form. The number and meaning of parameters depends on the selected functional form." \
    }
std::vector<double> mu_parameters;
// clang-format on

/// @brief Calculates the marginal utility based on the selected functional form
/// and parameters.
///
/// This function will be implemented in Step 3.
///
/// @param commods Vector of commodity names
/// @param prefs Vector of preference values
/// @return Calculated marginal utility value
double CalcMarginalUtility(std::vector<std::string> commods,
                          std::vector<double> prefs) const {
  // Implementation will be added in Step 3
  return 0.0;
}

// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_mu_functional_form = {0};
std::vector<int> cycpp_shape_mu_parameters = {0};


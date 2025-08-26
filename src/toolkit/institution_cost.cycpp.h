/// This includes the required header to add institution costs to archetypes.
/// One should only need to:
/// - '#include "toolkit/institution_cost.cycpp.h"' in the header of the
///    archetype class (as private)
/// - Add `InitEconParameters()` to `EnterNotify()` in the cc file of the
///   archetype class.

/// How to add parameters to this file:
/// 1. Add the pragma. A default value MUST be added to ensure backwards
///    compatibility.
/// 2. Edit the unordered_map called "econ_params"
///          i. add the desired parameter to the array {"name", value}
///         ii. the value of the pair should be the variable name exactly
/// 3. Add "std::vector<int> cycpp_shape_<param_name> = {0};" to the end of the
///    file with the other ones, reaplcing <param_name> with the name you put
///    in the econ_params array (again, must match exactly).

// clang-format off
#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Minimum acceptable rate of return", \
    "range": [0.0, 1.0], \
    "doc": "Minimum acceptable rate of return for the institution", \
    "units": "Dimensionless" \
    }
double minimum_acceptable_return_rate;

#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Corporate Income Tax Rate", \
    "range": [0.0, 1.0], \
    "doc": "Corporate income tax rate as decimal (1% --> 0.01)", \
    "units": "Dimensionless" \
    }
double corporate_income_tax_rate;

#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Bond-holder's Expected Rate of Return", \
    "range": [0.0, 1.0], \
    "doc": "Expected rate of return for bond holders as decimal (1% --> 0.01)", \
    "units": "Dimensionless" \
    }
double bond_holders_rate_of_return;

#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Fraction of Initial Investment from Bonds", \
    "range": [0.0, 1.0], \
    "doc": "Fraction of initial investment financed through bonds as decimal (1% --> 0.01)", \
    "units": "Dimensionless" \
    }
double fraction_bond_financing;

#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Share-holder's Expected Rate of Return", \
    "range": [0.0, 1.0], \
    "doc": "Expected rate of return for share holders as decimal (1% --> 0.01)", \
    "units": "Dimensionless" \
    }
double share_holders_rate_of_return;

#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Fraction of Initial Investment from Private Capital", \
    "range": [0.0, 1.0], \
    "doc": "Fraction of initial investment financed through private capital as decimal (1% --> 0.01)", \
    "units": "Dimensionless" \
    }
double fraction_private_capital;
// clang-format on    

// Must be done in a function so that we can access the user-defined values
std::unordered_map<std::string, double> GenerateParamList() const {
    std::unordered_map<std::string, double> econ_params {
        {"minimum_acceptable_return_rate", minimum_acceptable_return_rate},
        {"corporate_income_tax_rate", corporate_income_tax_rate},
        {"bond_holders_rate_of_return", bond_holders_rate_of_return},
        {"fraction_bond_financing", fraction_bond_financing},
        {"share_holders_rate_of_return", share_holders_rate_of_return},
        {"fraction_private_capital", fraction_private_capital}
    };

    return econ_params;
}
    
    
// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_minimum_acceptable_return_rate = {0};
std::vector<int> cycpp_shape_corporate_income_tax_rate = {0};
std::vector<int> cycpp_shape_bond_holders_rate_of_return = {0};
std::vector<int> cycpp_shape_fraction_bond_financing = {0};
std::vector<int> cycpp_shape_share_holders_rate_of_return = {0};
std::vector<int> cycpp_shape_fraction_private_capital = {0};
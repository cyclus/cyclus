/// This includes the required header to add facility costs to archetypes.
/// One should only need to:
/// - '#include "toolkit/facility_cost.cycpp.h"' in the header of the
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
  "uilabel": "Variable Cost Per Unit", \
  "doc": "Variable cost per unit of production (labor, materials, etc.). " \
         "The data from the Cost Basis Report represents a variable cost per " \
         "unit and can be used here.", \
  "units": "Unit of Currency/Unit of Production" \
  }
double variable_cost_per_unit;
// clang-format on

// Must be done in a function so that we can access the user-defined values
std::unordered_map<std::string, double> GenerateParamList() const override {
  std::unordered_map<std::string, double> econ_params{
      {"variable_cost_per_unit", variable_cost_per_unit}};

  return econ_params;
}

/// @brief Returns the sum of the variable cost per unit and the material cost 
/// per unit if available, and the default unit cost of kDefaultUnitCost if not.
///
/// @param material_cost_per_unit Per-unit cost of input materials
/// @return Sum of variable cost per unit and material cost per unit, or 
/// kDefaultUnitCost if not available
double CalcMarginalCost(double material_cost_per_unit) const {
  double variable_cost_per_unit;
  
  try {
    variable_cost_per_unit = GetEconParameter("variable_cost_per_unit");
  } catch (const std::exception& e) {
    LOG(cyclus::LEV_INFO1, "ValidateFacilityEconParameters")
        << prototype()
        << "failed to get variable cost per unit: " << e.what();
    return kDefaultUnitCost;
  }

  if (variable_cost_per_unit <= cyclus::eps()) {
    LOG(cyclus::LEV_WARN, "CalcMarginalCost")
          << prototype()
          << "has a very low (" << variable_cost_per_unit << ") variable cost per unit!";
  }

  return variable_cost_per_unit + material_cost_per_unit;
}



// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_variable_cost_per_unit = {0};
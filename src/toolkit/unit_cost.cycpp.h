/// This includes the required header to add facility costs to archetypes.
/// One should only need to:
/// - '#include "toolkit/unit_cost.cycpp.h"' in the header of the
///    archetype class (as private)

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
  "units": "Unit of Currency/Unit of Production", \
  "uitype": "range", \
  "range": [-CY_LARGE_DOUBLE, CY_LARGE_DOUBLE], \
  }
double unit_cost;

#pragma cyclus var { \
  "default": 0.0, \
  "uilabel": "Unit Cost Modifier", \
  "doc": "Requesting facility cost modifier to express preferences. In units" \
         "of currency. Setting this term arbitrarily removes economic meaning"\
         "from the reported arc_cost in Transactions. Does not affect costs" \
         "carried through facilities." ,\
  "units": "Unit of Currency/Unit of Production", \
  "uitype": "range", \
  "range": [-CY_LARGE_DOUBLE, CY_LARGE_DOUBLE], \
  }
double unit_cost_mod;
// clang-format on

// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_unit_cost = {0};
std::vector<int> cycpp_shape_unit_cost_mod = {0};
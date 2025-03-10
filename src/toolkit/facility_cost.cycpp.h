/// This includes the required header to add facility costs to archetypes.
/// One should only need to:
/// - '#include "toolkit/facility_cost.h"' in the main include section
///    of the header of the archetype class.
/// - '#include "toolkit/facility_cost.cycpp.h"' in the header of the 
///    archetype class (as private)
/// - Add `InitializeCosts()` to `EnterNotify()` in the cc file of the
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

#pragma cyclus var { \
"default": 0.0, \
"uilabel": "Capital cost required to build facility", \
"doc": "Capital cost required to build facility", \
"units": "$USD" \
}
double capital_cost;

#pragma cyclus var { \
"default": 0.0, \
"uilabel": "Annual O&M Cost in dollars", \
"doc": "Annual O&M Cost required to run facility in dollars", \
"units": "$USD" \
}
double operations_and_management;

#pragma cyclus var { \
    "default": 1.0, \
    "uilabel": "Estimated Useful lifetime of facility for economic purposes in years", \
    "doc": "Estimate on how long the facility will be active for economic purposes", \
    "units": "years" \
    }
double facility_lifetime;

#pragma cyclus var { \
    "default": 1.0, \
    "uilabel": "Annual fractional increase of initial facility capacity", \
    "doc": "Fraction of initial capacity by which facility capacity increases/decreases each year", \
    "units": "Dimensionless" \
    }
double capacity_decline_factor;

#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Cost in dollars of labor required to produce one unit of production", \
    "doc": "Cost in dollars of labor required to produce one unit of production", \
    "units": "$USD" \
    }
double per_unit_labor_cost;

#pragma cyclus var { \
    "default": 0.04, \
    "uilabel": "Compounding fractional annual increase in the cost of labor as a decimal", \
    "doc": "Cost in dollars of labor required to produce one unit of production", \
    "units": "Dimensionless" \
    }
double annual_labor_cost_increase_factor;



// Must be done in a function so that we can access the user-defined values
std::unordered_map<std::string, double> InitializeParmList() {
    std::unordered_map<std::string, double> econ_params {
        {"capital_cost", capital_cost},
        {"operations_and_management", operations_and_management},
        {"facility_lifetime", facility_lifetime},
        {"capacity_decline_factor", capacity_decline_factor},
        {"per_unit_labor_cost", per_unit_labor_cost}
    };

    return econ_params;
}

// Add the financial parameters to the class
void InitializeCosts() { 
    std::unordered_map<std::string, double> econ_params = InitializeParmList();
    for (const auto& parameter : econ_params) {
        this->SetEconParameter(parameter.first, parameter.second);
    }
}

// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_capital_cost = {0};
std::vector<int> cycpp_shape_operations_and_management = {0};
std::vector<int> cycpp_shape_facility_lifetime = {0};
std::vector<int> cycpp_shape_capacity_decline_factor = {0};
std::vector<int> cycpp_shape_per_unit_labor_cost = {0};
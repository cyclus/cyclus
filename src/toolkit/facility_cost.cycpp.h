/// This includes the required header to add facility costs to archetypes.
/// One should only need to:
/// - '#include "toolkit/facility_cost.h"' in the main include section
///    of the header of the archetype class.
/// - '#include "toolkit/facility_cost.cycpp.h"' in the header of the 
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

//cyclus::toolkit::FacilityCost cost;

#pragma cyclus var { \
"default": 0.0, \
"uilabel": "Capital cost required to build facility", \
"doc": "Capital cost required to build facility" \
}
double capital_cost;

#pragma cyclus var { \
"default": 0.0, \
"uilabel": "O&M Cost", \
"doc": "monthly O&M Cost required to run facility" \
}
double operations_and_management;

// Must be done in a function so that we can access the user-defined values
std::unordered_map<std::string, double> InitializeParmList() {
    std::unordered_map<std::string, double> econ_params {
        {"capital_cost", capital_cost},
        {"operations_and_management", operations_and_management}
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
// remove.
 std::vector<int> cycpp_shape_capital_cost = {0};
 std::vector<int> cycpp_shape_operations_and_management = {0};
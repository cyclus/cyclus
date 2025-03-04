/// This includes the required header to add institution costs to archetypes.
/// One should only need to:
/// - '#include "toolkit/institution_cost.h"' in the main include section
///    of the header of the archetype class.
/// - '#include "toolkit/institution_cost.cycpp.h"' in the header of the 
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

//cyclus::toolkit::FacilityCost cost;

#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Minimum acceptable rate of return", \
    "doc": "Minimum acceptable rate of return for the institution" \
    }
    double minimum_acceptable_return_rate;
    
    
    // Must be done in a function so that we can access the user-defined values
    std::unordered_map<std::string, double> InitializeParmList() {
        std::unordered_map<std::string, double> econ_params {
            {"minimum_acceptable_return_rate", minimum_acceptable_return_rate}
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
     std::vector<int> cycpp_shape_minimum_acceptable_return_rate = {0};
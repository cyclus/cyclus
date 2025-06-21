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

#include "economic_parameter.h"

#pragma cyclus var {"default" : 0.0,                                       \
                   "uilabel" : "Capital cost required to build facility", \
                   "doc" : "Capital cost required to build facility",     \
                   "units" : "Unit of Currency" }
double capital_cost;

#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Property Tax Rate as decimal", \
    "range": [0.0, 1.0], \
    "doc": "Property tax rate for this facility as decimal (1% --> 0.01)" \
    }
double property_tax_rate;

#pragma cyclus var {                                                \
    "default" : 0.0, "uilabel" : "Annual O&M Cost",     \
    "doc" : "Annual O&M Cost required to run facility", \
    "units" : "Unit of Currency" }
double operations_and_maintenance;

#pragma cyclus var { \
    "default": 1.0, \
    "uilabel": "Estimated Useful lifetime of facility for economic purposes in years", \
    "doc": "Estimate on how long the facility will be active for economic purposes", \
    "units": "years" \
    }
double facility_operational_lifetime;

#pragma cyclus var { \
    "default": 1.0, \
    "uilabel": "Taxable lifetime of facility for economic purposes in years", \
    "doc": "How long the facility will be depreciating their initial investment", \
    "units": "years" \
    }
double facility_depreciation_lifetime;

#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Cost of labor required to produce one unit of production", \
    "doc": "Cost of labor required to produce one unit of production", \
    "units": "Unit of Currency" \
    }
double per_unit_labor_cost;

#pragma cyclus var { \
    "default": -1.0, \
    "uilabel": "Cost in dollars of one unit of production", \
    "doc": "(optional) Hook to bypass LCP calculation and provide a cost in dollars", \
    "units": "Dimensionless" \
    }
double cost_override;


// Must be done in a function so that we can access the user-defined values
std::vector<EconParameter> GenerateParamList() const override {
  std::vector<EconParameter> econ_params{
      {"capital_cost", capital_cost, CostCategory::Depreciable},
      {"property_tax_rate", property_tax_rate, CostCategory::Tax},
      {"operations_and_maintenance", operations_and_maintenance, CostCategory::Fixed},
      {"facility_operational_lifetime", facility_operational_lifetime, CostCategory::Time},
      {"facility_depreciation_lifetime", facility_depreciation_lifetime, CostCategory::Time},
      {"per_unit_labor_cost", per_unit_labor_cost, CostCategory::Variable}};

  return econ_params;
}

double CalculateUnitCost(double production_capacity, double units_to_produce, 
    double input_cost) const {
    
    if (cost_override > 0) {
        return cost_override * units_of_production + input_cost;
    }

        // Economic Parameters (declared like this because of scoping with try{})
    double return_rate;
    double cap_cost;
    int operational_lifetime;
    double operations_maintenance;
    double labor_cost;
    double corporate_tax;
    double property_tax;
    int taxable_lifetime;

    // This allows us to exit gracefully if we can't find parameters
    try {
        return_rate = parent()->GetEconParameter("minimum_acceptable_return_rate");
        cap_cost = GetEconParameter("capital_cost");
        operational_lifetime = static_cast<int>(GetEconParameter("facility_operational_lifetime"));
        operations_maintenance = GetEconParameter("operations_and_maintenance");
        labor_cost = GetEconParameter("per_unit_labor_cost");
        corporate_tax = parent()->GetEconParameter("corporate_income_tax_rate");
        property_tax = parent()->parent()->GetEconParameter("property_tax_rate");
        taxable_lifetime = static_cast<int>(GetEconParameter("facility_taxable_lifetime"));
    } 
    catch (const std::exception& e) {
        // If any of the above functions fail to get their parameters, return 1
        // to default to old pref logic
        LOG(cyclus::LEV_INFO1, "GetCost") << prototype() 
                                        << "failed to get financial_data_: "
                                        << e.what();
        return 1;
    }

    double timesteps_per_year = kDefaultTimeStepDur * 12 / context()->dt();
    double annual_production = production_capacity * timesteps_per_year;


    
}

// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_capital_cost = {0};
std::vector<int> cycpp_shape_property_tax_rate = {0};
std::vector<int> cycpp_shape_operations_and_maintenance = {0};
std::vector<int> cycpp_shape_facility_operational_lifetime = {0};
std::vector<int> cycpp_shape_facility_depreciation_lifetime = {0};
std::vector<int> cycpp_shape_per_unit_labor_cost = {0};
std::vector<int> cycpp_shape_cost_override = {0};
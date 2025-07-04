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
double annual_operations_and_maintenance;

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

// We maybe want this to be more like a line-item not per unit?
#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Annual cost of labor", \
    "doc": "Annual cost of labor", \
    "units": "Unit of Currency" \
    }
double annual_labor_cost;

#pragma cyclus var { \
    "default": -1.0, \
    "uilabel": "Cost in dollars of one unit of production", \
    "doc": "(optional) Hook to bypass LCP calculation and provide a cost in dollars", \
    "units": "Dimensionless" \
    }
double cost_override;


// Must be done in a function so that we can access the user-defined values
std::unordered_map<std::string, double> GenerateParamList() const override {
  std::unordered_map<std::string, double> econ_params{
      {"capital_cost", capital_cost},
      {"property_tax_rate", property_tax_rate},
      {"annual_operations_and_maintenance", annual_operations_and_maintenance},
      {"facility_operational_lifetime", facility_operational_lifetime},
      {"facility_depreciation_lifetime", facility_depreciation_lifetime},
      {"annual_labor_cost", annual_labor_cost}};

  return econ_params;
}

double CalculateBidCost(double production_capacity, double units_to_produce, 
    double input_cost) const {
    
    // Check if there's a cost override, and if so, use that
    if (cost_override > 0) {
        return cost_override * units_of_production + input_cost;
    }

    // Economic Parameters (declared like this because of scoping with try{})
    double return_rate;
    double cap_cost;
    double annual_operations_and_maintenance;
    double annual_labor_cost;
    int operational_lifetime;
    int taxable_lifetime;
    double corporate_tax_rate;
    double property_tax_rate;

    // This allows us to exit gracefully if we can't find parameters. Separated
    // by Facility/Institution/Region for more verbose errors.
    try {
        capital_cost = GetEconParameter("capital_cost");
        annual_operations_and_maintenance = GetEconParameter("annual_operations_and_maintenance");
        annual_labor_cost = GetEconParameter("annual_labor_cost");
        operational_lifetime = static_cast<int>(GetEconParameter("facility_operational_lifetime"));
        taxable_lifetime = static_cast<int>(GetEconParameter("facility_taxable_lifetime"));
    } 
    catch (const std::exception& e) {
        LOG(cyclus::LEV_INFO1, "CalculateBidCost") << prototype() 
                                        << "failed to get financial_data_: "
                                        << e.what();
        return kDefaultBidCost;
    }

    try {
        return_rate = parent()->GetEconParameter("minimum_acceptable_return_rate");
        corporate_tax_rate = parent()->GetEconParameter("corporate_income_tax_rate");
    }
    catch (const std::exception& e) {
        LOG(cyclus::LEV_INFO1, "CalculateBidCost") << prototype() 
                                        << "failed to get financial_data_ from: " 
                                        << parent()->prototype()
                                        << e.what();
        return kDefaultBidCost;
    }

    try {
        property_tax_rate = parent()->parent()->GetEconParameter("property_tax_rate");
    }
    catch (const std::exception& e) {
        LOG(cyclus::LEV_INFO1, "CalculateBidCost") << prototype()
                                            << "failed to get financial_data_ from: "
                                            << parent()->parent()->prototype()
                                            << e.what();

        return kDefaultBidCost;
    }

    // Once that other time-step related PR gets merged this can be cyclusYear
    double timesteps_per_year = kDefaultTimeStepDur * 12 / context()->dt();
    double annual_production = production_capacity * timesteps_per_year;

    // This is my way of keeping the categories. New costs can be added to these
    // as needed and it should be "easy" to do, but still require some thinking
    double total_dep = capital_cost;
    double total_annual_fixed = annual_operations_and_maintenance;
    double total_annual_variable = annual_labor_cost;
    
    double property_tax = property_tax_rate * capital_cost;

    // Adjust for Corp. Income Tax and Depreciation
    tax_shield = PV(taxable_lifetime, return_rate, 0, 
        total_dep * corporate_tax_rate / taxable_lifetime);

    annualized_depreciable = PMT(operational_lifetime, return_rate, 
        total_dep - tax_shield, 0);
    
    double unit_cost = (annualized_depreciable + total_annual_fixed + 
        total_annual_variable + property_tax) / annual_production;
    
    double bid_cost = unit_cost * units_to_produce + input_cost;

    // Protects against divide by zero in pref = 1/BidCost
    return bid_cost != 0 ? bid_cost : kDefaultBidCost;

    
}

// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_capital_cost = {0};
std::vector<int> cycpp_shape_property_tax_rate = {0};
std::vector<int> cycpp_shape_annual_operations_and_maintenance = {0};
std::vector<int> cycpp_shape_facility_operational_lifetime = {0};
std::vector<int> cycpp_shape_facility_depreciation_lifetime = {0};
std::vector<int> cycpp_shape_annual_labor_cost = {0};
std::vector<int> cycpp_shape_cost_override = {0};
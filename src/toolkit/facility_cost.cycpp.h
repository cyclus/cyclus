/// This includes the required header to add facility costs to archetypes.
/// One should only need to:
/// - '#include "toolkit/facility_cost.cycpp.h"' in the header of the
///    archetype class (as private)
/// - Add `InitializeCosts()` to `EnterNotify()` in the cc file of the
///   archetype class, and then `GetCost()` should become available.

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
                   "units" : "$USD" }
double capital_cost;

#pragma cyclus var {                                                \
    "default" : 0.0, "uilabel" : "Annual O&M Cost in dollars",     \
    "doc" : "Annual O&M Cost required to run facility in dollars", \
    "units" : "$USD" }
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
double facility_taxable_lifetime;

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

#pragma cyclus var { \
    "default": -1.0, \
    "uilabel": "Cost in dollars of one unit of production", \
    "doc": "(optional) Hook to bypass LCP calculation and provide a cost in dollars", \
    "units": "Dimensionless" \
    }
double cost_override;

// This is for testing right now, need to think/talk about the "right" way to 
// do this...
double avg_per_unit_cost = 0;
double total_qty_purchased = 0;


// Must be done in a function so that we can access the user-defined values
std::unordered_map<std::string, double> InitializeParamList() const override {
  std::unordered_map<std::string, double> econ_params{
      {"capital_cost", capital_cost},
      {"operations_and_maintenance", operations_and_maintenance},
      {"facility_operational_lifetime", facility_operational_lifetime},
      {"facility_taxable_lifetime", facility_taxable_lifetime},
      {"capacity_decline_factor", capacity_decline_factor},
      {"per_unit_labor_cost", per_unit_labor_cost},
      {"annual_labor_cost_increase_factor", annual_labor_cost_increase_factor}};

  return econ_params;
}

// Added production_capacity_per_timestep to allow units_of_production to
// take the actual production of that bid into account... Not totally sure
// how I feel about all this at the moment.
double GetCost(double production_capacity_per_timestep, 
               double units_of_production, double input_cost) {

  if (cost_override > 0) {
    // Not totally sure how to levelize this yet, just sort of testing things...
    return cost_override * units_of_production + input_cost;
  }

  // Economic Parameters (declared like this because of scoping with try{})
  double r;
  double cdf;
  double v;
  int T;
  double OM;
  double labor_cost;
  double alpha;
  double p;
  double depreciation_constant;
  int T_hat;

  // This allows GetCost() to exit gracefully if it can't find parameters
  try {
    r = parent()->GetEconParameter("minimum_acceptable_return_rate");
    cdf = GetEconParameter("capacity_decline_factor");
    v = GetEconParameter("capital_cost");
    T = static_cast<int>(GetEconParameter("facility_operational_lifetime"));
    OM = GetEconParameter("operations_and_maintenance");
    labor_cost = GetEconParameter("per_unit_labor_cost");
    alpha = parent()->GetEconParameter("corporate_income_tax_rate");
    p = parent()->parent()->GetEconParameter("property_tax_rate");
    depreciation_constant = parent()->GetEconParameter("depreciation_constant");
    T_hat = static_cast<int>(GetEconParameter("facility_taxable_lifetime"));
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
  double k = production_capacity_per_timestep * timesteps_per_year;
  double variable_cost = labor_cost + input_cost;

  double property_tax = p * v;
  double F = OM;
  double L = PresentWorthGrowingAnnuity(r, T, cdf) * k;

  double c = v / L;
  double f = F * PresentWorthGrowingAnnuity(r, T) / L;
  double w = variable_cost * k * PresentWorthGrowingAnnuity(r, T, cdf) / L;
  double delta = ComputeTaxFactor(depreciation_constant/T_hat, v, T_hat, r, alpha);

  double cost = w + f + c * (property_tax + delta); 

  // Since pref = 1/cost (for now) we CANNOT return 0
  return cost != 0 ? (cost * units_of_production) : 1;
}

// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_capital_cost = {0};
std::vector<int> cycpp_shape_operations_and_maintenance = {0};
std::vector<int> cycpp_shape_facility_operational_lifetime = {0};
std::vector<int> cycpp_shape_facility_taxable_lifetime = {0};
std::vector<int> cycpp_shape_capacity_decline_factor = {0};
std::vector<int> cycpp_shape_per_unit_labor_cost = {0};
std::vector<int> cycpp_shape_annual_labor_cost_increase_factor = {0};
std::vector<int> cycpp_shape_cost_override = {0};
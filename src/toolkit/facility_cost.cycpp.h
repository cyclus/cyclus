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
    "default" : 0.0, "uilabel" : "Annual Operations and Maintenance Cost",     \
    "doc" : "Annual Operations and Maintenance Cost required to run facility", \
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

#pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Property Insurance Rate as decimal", \
    "range": [0.0, 1.0], \
    "doc": "Property insurance rate for this facility as decimal (1% --> 0.01)" \
    }
double property_insurance_rate;
// clang-format on

// Must be done in a function so that we can access the user-defined values
std::unordered_map<std::string, double> GenerateParamList() const override {
  std::unordered_map<std::string, double> econ_params{
      {"capital_cost", capital_cost},
      {"property_tax_rate", property_tax_rate},
      {"annual_operations_and_maintenance", annual_operations_and_maintenance},
      {"facility_operational_lifetime", facility_operational_lifetime},
      {"facility_depreciation_lifetime", facility_depreciation_lifetime},
      {"annual_labor_cost", annual_labor_cost},
      {"property_insurance_rate", property_insurance_rate}};

  return econ_params;
}

/// @brief Calculates the levelized unit cost of production, accounting for
/// capital depreciation, O&M, labor, property taxes, and input costs.
/// 
/// unit_cost = cost_override if cost_override > 0, otherwise unit_cost = production_cost + material_cost
/// 
/// Where:
/// - production_cost = levelized_fixed_costs/units_produced_annually + levelized_variable_costs + (initial_investment/units_produced_annually) * [property_tax_insurance_rate + (1/(1-income_tax_rate)) * PMT(facility_lifetime, tax_modified_rate_of_return, 1, 0) - (1/facility_lifetime) * (income_tax_rate/(1-income_tax_rate))]
/// - material_cost = Unit Cost of Material (weighted average of input material unit values)
/// - units_produced_annually = production_capacity * timesteps_per_year
/// - levelized_fixed_costs = annual_operations_and_maintenance
/// - levelized_variable_costs = annual_labor_cost
/// - initial_investment = capital_cost
/// - property_and_insurance_rate = property_tax_rate + property_insurance_rate
/// - tax_modified_rate_of_return = (1-income_tax_rate)*bond_rate*bond_fraction + shareholder_rate*shareholder_fraction
/// - income_tax_rate = Income Tax Rate
/// - facility_lifetime = facility_depreciation_lifetime
///
/// The model assumes straight-line depreciation over the facility lifetime.
///
/// @param production_capacity Maximum throughput per timestep
/// @param units_to_produce Number of units produced in the batch
/// @param input_cost (Optional) Total cost of input materials used in the batch
/// @return Estimated levelized cost to produce one unit
double CalculateUnitCost(double production_capacity, double units_to_produce,
                         double input_cost_per_unit = 0.0) const {
  // Check if there's a cost override, and if so, use that
  if (cost_override > 0) {
    return cost_override + input_cost_per_unit;
  }

  // Economic Parameters (required for the try catch block)
  double initial_investment;
  double facility_lifetime;
  double levelized_fixed_costs;
  double levelized_variable_costs;
  double property_and_insurance_rate;
  double tax_modified_rate_of_return;
  double income_tax_rate;
  double bond_rate;
  double bond_fraction;
  double shareholder_rate;
  double shareholder_fraction;
  double property_insurance_rate;

  // Get facility-level parameters
  try {
    initial_investment = GetEconParameter("capital_cost");
    facility_lifetime = GetEconParameter("facility_depreciation_lifetime");
    property_insurance_rate = GetEconParameter("property_insurance_rate");

    // Note: Since our fixed and variable costs are the same every timestep, we
    // call them levelized here. If that changes in the future, we need to 
    // change this to reflect actual levelization. Additionally, if more
    // fixed and variable costs are added in the future, we need to change this
    // to reflect that as well.
    levelized_fixed_costs = GetEconParameter("annual_operations_and_maintenance");
    levelized_variable_costs = GetEconParameter("annual_labor_cost");
    
  } catch (const std::exception& e) {
    LOG(cyclus::LEV_INFO1, "CalculateUnitCost")
        << prototype() << "failed to get facility financial_data_: " << e.what();
    return kDefaultUnitCost;
  }

  // Get institution-level parameters
  try {
    income_tax_rate = parent()->GetEconParameter("corporate_income_tax_rate");
    bond_rate = parent()->GetEconParameter("bond_holders_rate_of_return");
    bond_fraction = parent()->GetEconParameter("fraction_bond_financing");
    shareholder_rate = parent()->GetEconParameter("share_holders_rate_of_return");
    shareholder_fraction = parent()->GetEconParameter("fraction_private_capital");
  } catch (const std::exception& e) {
    LOG(cyclus::LEV_INFO1, "CalculateUnitCost")
        << prototype()
        << "failed to get institution financial_data_: " << e.what();
    return kDefaultUnitCost;
  }

  // Get region-level parameters
  try {
    double property_tax_rate = parent()->parent()->GetEconParameter("property_tax_rate");
    property_and_insurance_rate = property_tax_rate + property_insurance_rate;
  } catch (const std::exception& e) {
    LOG(cyclus::LEV_INFO1, "CalculateUnitCost")
        << prototype() << "failed to get region financial_data_: " << e.what();
    return kDefaultUnitCost;
  }

  // U = throughput * timesteps_per_year
  double timesteps_per_year = cyclusYear / context()->dt();
  double units_produced_annually = production_capacity * timesteps_per_year;

  // x = (1-τ)*r_b*f_b + r_s*f_s
  tax_modified_rate_of_return = (1 - income_tax_rate) * bond_rate * bond_fraction + shareholder_rate * shareholder_fraction;

  // c_j = F_bar/U + V_bar + (I_0/U) * [p + (1/(1-τ)) * PMT(N,x,1,0) - (1/N) * (τ/(1-τ))]
  double fixed_cost_per_unit = levelized_fixed_costs / units_produced_annually;
  double variable_cost_per_unit = levelized_variable_costs;
  double capital_investment_per_unit = initial_investment / units_produced_annually;

  double depreciation_tax_shield = income_tax_rate / (1.0 - income_tax_rate) / facility_lifetime;
  double capital_recovery_factor = PMT(facility_lifetime, tax_modified_rate_of_return, 1.0, 0.0) / (1.0 - income_tax_rate);
  double capital_cost_per_unit = capital_investment_per_unit * (property_and_insurance_rate + capital_recovery_factor - depreciation_tax_shield);
  double production_cost = fixed_cost_per_unit + variable_cost_per_unit + capital_cost_per_unit;

  // c_u = c_j + c_M = production_cost + input_cost_per_unit
  double unit_cost = production_cost + input_cost_per_unit;

  // Protects against divide by zero in pref = 1/unit_cost
  return unit_cost != 0 ? unit_cost : kDefaultUnitCost;
}

double CalculateUnitPrice(double production_capacity, double units_to_produce,
                          double input_cost_per_unit = 0.0) const {
  // Default implementation
  return CalculateUnitCost(production_capacity, units_to_produce, input_cost_per_unit);
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
std::vector<int> cycpp_shape_property_insurance_rate = {0};
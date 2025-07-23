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
// clang-format on

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

  /// @brief Calculates the levelized unit cost of production, accounting for
  /// capital depreciation, O&M, labor, property taxes, and input costs.
  ///
  /// This function estimates the cost to produce a single unit of output using:
  /// - Capital cost amortized over operational life, adjusted for tax shield
  /// - Annual fixed costs (O&M, labor)
  /// - Property taxes as a percent of capital cost
  /// - Input cost allocated per unit in the batch
  ///
  /// The cost model assumes:
  /// - Straight-line depreciation over the taxable lifetime
  /// - Discounting via the minimum acceptable return rate
  /// - Property tax as a simple annual percent of capital cost
  /// - All values are in real (non-inflated) terms
  ///
  /// Equations used (where applicable):
  ///   - Annual production: Q = P_cap * Y, with Y = (cyclusYear / dt)
  ///   - Property tax: T_prop = t_prop * C_cap
  ///   - Tax shield from depreciation:
  ///       TaxShield = PV(n=L_tax, i=r, F=0, A=(C_cap * t_corp / L_tax))
  ///   - Annualized net capital cost:
  ///       A_depr = PMT(n=L_op, i=r, P=(C_cap - TaxShield), F=0)
  ///   - Total annualized cost:
  ///       C_annual = A_depr + C_O&M + C_labor + T_prop
  ///   - Unit cost: C_unit = (C_annual / Q) + (C_input / u)
  ///
  /// If a cost override is specified, the function returns:
  ///   C_unit = cost_override + (C_input / u)
  ///
  /// If the computed unit cost is zero (e.g., divide-by-zero), a default
  /// value kDefaultUnitCost is returned instead.
  ///
  /// @param production_capacity Maximum throughput per timestep
  /// @param units_to_produce Number of units produced in the batch
  /// @param input_cost (Optional) Total cost of input materials used in the batch
  /// @return Estimated levelized cost to produce one unit
double CalculateUnitCost(double production_capacity, double units_to_produce,
                         double input_cost = 0.0) const {
  // Check if there's a cost override, and if so, use that
  if (cost_override > 0) {
    return cost_override + input_cost / units_to_produce;
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
    cap_cost = GetEconParameter("capital_cost");
    annual_operations_and_maintenance =
        GetEconParameter("annual_operations_and_maintenance");
    annual_labor_cost = GetEconParameter("annual_labor_cost");
    operational_lifetime =
        static_cast<int>(GetEconParameter("facility_operational_lifetime"));
    taxable_lifetime =
        static_cast<int>(GetEconParameter("facility_taxable_lifetime"));
  } catch (const std::exception& e) {
    LOG(cyclus::LEV_INFO1, "CalculateUnitCost")
        << prototype() << "failed to get financial_data_: " << e.what();
    return kDefaultUnitCost;
  }

  try {
    return_rate = parent()->GetEconParameter("minimum_acceptable_return_rate");
    corporate_tax_rate =
        parent()->GetEconParameter("corporate_income_tax_rate");
  } catch (const std::exception& e) {
    LOG(cyclus::LEV_INFO1, "CalculateUnitCost")
        << prototype()
        << "failed to get financial_data_ from: " << parent()->prototype()
        << e.what();
    return kDefaultUnitCost;
  }

  try {
    property_tax_rate =
        parent()->parent()->GetEconParameter("property_tax_rate");
  } catch (const std::exception& e) {
    LOG(cyclus::LEV_INFO1, "CalculateUnitCost")
        << prototype() << "failed to get financial_data_ from: "
        << parent()->parent()->prototype() << e.what();

    return kDefaultUnitCost;
  }

  double timesteps_per_year = cyclusYear / context()->dt();
  double annual_production = production_capacity * timesteps_per_year;

  // This is my way of keeping the categories. New costs can be added to these
  // as needed and it should be "easy" to do, but still require some thinking
  double total_dep = cap_cost;
  double total_annual_fixed = annual_operations_and_maintenance;
  double total_annual_variable = annual_labor_cost;

  double property_tax = property_tax_rate * cap_cost;

  // Adjust for Corp. Income Tax and Depreciation
  double tax_shield = PV(taxable_lifetime, return_rate, 0,
                         total_dep * corporate_tax_rate / taxable_lifetime);

  double annualized_depreciable =
      PMT(operational_lifetime, return_rate, total_dep - tax_shield, 0);

  double unit_production_cost = (annualized_depreciable + total_annual_fixed +
                                 total_annual_variable + property_tax) /
                                annual_production;

  double unit_cost = unit_production_cost + input_cost / units_to_produce;

  // Protects against divide by zero in pref = 1/unit_cost
  return unit_cost != 0 ? unit_cost : kDefaultUnitCost;
}

double CalculateUnitPrice(double production_capacity, double units_to_produce,
                          double input_cost = 0.0) const {
  // Default implementation
  return CalculateUnitCost(production_capacity, units_to_produce, input_cost);
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
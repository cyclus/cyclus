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
                   "doc" : "Total overnight capital cost required to build facility",     \
                   "units" : "Unit of Currency" }
double capital_cost;

#pragma cyclus var {                                                \
    "default" : 0.0, "uilabel" : "Annual Fixed Costs",     \
    "doc" : "Annual fixed costs (operations, maintenance, etc., excluding property tax and insurance) required to run facility", \
    "units" : "Unit of Currency/year" }
double annual_fixed_costs;

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
    "uilabel": "Variable Cost Per Unit", \
    "doc": "Variable cost per unit of production (labor, materials, etc. that vary with production)", \
    "units": "Unit of Currency/Unit of Production" \
    }
double variable_cost_per_unit;

#pragma cyclus var { \
    "default": -1.0, \
    "uilabel": "Non-material cost override in dollars of one unit of production", \
    "doc": "(optional) Hook to bypass LCP calculation and provide a cost in dollars. Should NOT include material costs, which are added later", \
    "units": "unit of currency/unit of production (eg. $/kg)" \
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
      {"annual_fixed_costs", annual_fixed_costs},
      {"facility_operational_lifetime", facility_operational_lifetime},
      {"facility_depreciation_lifetime", facility_depreciation_lifetime},
      {"variable_cost_per_unit", variable_cost_per_unit},
      {"property_insurance_rate", property_insurance_rate}};

  return econ_params;
}

// =============================================================================
// Economic Parameter Retrieval Functions
// =============================================================================

/// @brief Retrieves facility-level economic parameters
/// @param initial_investment Output parameter for capital cost
/// @param facility_lifetime Output parameter for depreciation lifetime
/// @param property_insurance_rate Output parameter for property insurance rate
/// @param levelized_fixed_costs Output parameter for annual fixed costs
/// @param variable_cost_per_unit Output parameter for variable cost per unit
/// @return true if successful, false otherwise
bool GetFacilityEconParameters(double& initial_investment,
                                double& facility_lifetime,
                                double& property_insurance_rate,
                                double& levelized_fixed_costs,
                                double& variable_cost_per_unit) const {
  try {
    initial_investment = GetEconParameter("capital_cost");
    facility_lifetime = GetEconParameter("facility_depreciation_lifetime");
    property_insurance_rate = GetEconParameter("property_insurance_rate");

    // Note: Since our fixed costs are the same every timestep, we
    // call them levelized here. If that changes in the future, we need to
    // change this to reflect actual levelization. Variable costs are provided
    // directly as per-unit costs.
    levelized_fixed_costs =
        GetEconParameter("annual_fixed_costs");
    variable_cost_per_unit = GetEconParameter("variable_cost_per_unit");
    return true;
  } catch (const std::exception& e) {
    LOG(cyclus::LEV_INFO1, "GetFacilityEconParameters")
        << prototype()
        << "failed to get facility financial_data_: " << e.what();
    return false;
  }
}

/// @brief Retrieves institution-level economic parameters
/// @param income_tax_rate Output parameter for corporate income tax rate
/// @param bond_rate Output parameter for bond holders rate of return
/// @param bond_fraction Output parameter for fraction bond financing
/// @param shareholder_rate Output parameter for share holders rate of return
/// @param shareholder_fraction Output parameter for fraction private capital
/// @param discount_rate_override Output parameter for discount rate override
/// @return true if successful, false otherwise
bool GetInstitutionEconParameters(double& income_tax_rate,
                                   double& bond_rate,
                                   double& bond_fraction,
                                   double& shareholder_rate,
                                   double& shareholder_fraction,
                                   double& discount_rate_override) const {
  try {
    income_tax_rate = parent()->GetEconParameter("corporate_income_tax_rate");
    bond_rate = parent()->GetEconParameter("bond_holders_rate_of_return");
    bond_fraction = parent()->GetEconParameter("fraction_bond_financing");
    shareholder_rate =
        parent()->GetEconParameter("share_holders_rate_of_return");
    shareholder_fraction =
        parent()->GetEconParameter("fraction_private_capital");
    discount_rate_override =
        parent()->GetEconParameter("discount_rate_override");
    return true;
  } catch (const std::exception& e) {
    LOG(cyclus::LEV_INFO1, "GetInstitutionEconParameters")
        << prototype()
        << "failed to get institution financial_data_: " << e.what();
    return false;
  }
}

/// @brief Retrieves region-level economic parameters
/// @param property_tax_rate Output parameter for property tax rate from region
/// @return true if successful, false otherwise
bool GetRegionEconParameters(double& property_tax_rate) const {
  try {
    property_tax_rate =
        parent()->parent()->GetEconParameter("property_tax_rate");
    return true;
  } catch (const std::exception& e) {
    LOG(cyclus::LEV_INFO1, "GetRegionEconParameters")
        << prototype() << "failed to get region financial_data_: " << e.what();
    return false;
  }
}

// =============================================================================
// Cost Calculation Functions
// =============================================================================

/// @brief Calculates the number of units produced annually
/// @param production_capacity Maximum throughput per timestep
/// @return Units produced annually
double CalcUnitsProducedAnnually(double production_capacity) const {
  double timesteps_per_year = cyclusYear / context()->dt();
  return production_capacity * timesteps_per_year;
}

/// @brief Calculates the post tax weighted average cost of capital (WACC), also known as
/// tax-modified rate of return
/// Formula: (1-τ)*r_b*f_b + r_s*f_s
/// This is the discount rate used in capital cost calculations. Can be overridden
/// by setting discount_rate_override > 0 at the institution level.
/// @param income_tax_rate Corporate income tax rate
/// @param bond_rate Bond holders rate of return
/// @param bond_fraction Fraction of bond financing
/// @param shareholder_rate Share holders rate of return
/// @param shareholder_fraction Fraction of private capital
/// @return Tax-modified rate of return (WACC)
double CalcTaxModifiedRateOfReturn(double income_tax_rate,
                                    double bond_rate,
                                    double bond_fraction,
                                    double shareholder_rate,
                                    double shareholder_fraction) const {
  return (1 - income_tax_rate) * bond_rate * bond_fraction +
         shareholder_rate * shareholder_fraction;
}

/// @brief Calculates fixed cost per unit
/// @param levelized_fixed_costs Annual fixed costs
/// @param units_produced_annually Number of units produced per year
/// @return Fixed cost per unit
double CalcFixedCostPerUnit(double levelized_fixed_costs,
                             double units_produced_annually) const {
  if (units_produced_annually == 0) {
    return 0.0;
  }
  return levelized_fixed_costs / units_produced_annually;
}

/// @brief Calculates capital cost per unit
/// Formula: (I_0/U) * [p + (1/(1-τ)) * PMT(N,x,1,0) - (1/N) * (τ/(1-τ))]
/// where:
/// - I_0 = initial investment
/// - U = units produced annually
/// - p = property and insurance rate
/// - τ = income tax rate
/// - N = facility lifetime
/// - x = tax-modified rate of return
/// @param initial_investment Capital cost
/// @param units_produced_annually Number of units produced per year
/// @param facility_lifetime Facility depreciation lifetime in years
/// @param income_tax_rate Corporate income tax rate
/// @param tax_modified_rate_of_return Tax-modified rate of return
/// @param property_and_insurance_rate Combined property tax and insurance rate
/// @return Capital cost per unit
double CalcCapitalCostPerUnit(double initial_investment,
                               double units_produced_annually,
                               double facility_lifetime,
                               double income_tax_rate,
                               double tax_modified_rate_of_return,
                               double property_and_insurance_rate) const {
  if (units_produced_annually == 0) {
    return 0.0;
  }

  double capital_investment_per_unit =
      initial_investment / units_produced_annually;

  double depreciation_tax_shield =
      income_tax_rate / (1.0 - income_tax_rate) / facility_lifetime;

  double capital_recovery_factor =
      PMT(facility_lifetime, tax_modified_rate_of_return, 1.0, 0.0) /
      (1.0 - income_tax_rate);

  return capital_investment_per_unit *
         (property_and_insurance_rate + capital_recovery_factor -
          depreciation_tax_shield);
}

/// @brief Calculates marginal cost per unit (variable + material, excluding
/// fixed and capital costs)
/// Marginal cost represents the cost of producing one additional unit,
/// excluding fixed and capital costs which are sunk.
/// @param variable_cost_per_unit Variable cost per unit
/// @param material_cost_per_unit Material cost per unit
/// @return Marginal cost per unit
double CalcMarginalCost(double variable_cost_per_unit,
                        double material_cost_per_unit) const {
  return variable_cost_per_unit + material_cost_per_unit;
}

// =============================================================================
// Main Cost Calculation Function
// =============================================================================

/// @brief Calculates the levelized unit cost of production, accounting for
/// capital depreciation, fixed costs, variable costs, property taxes, and input costs.
///
/// unit_cost = cost_override + material_cost if cost_override > 0, otherwise unit_cost =
/// production_cost + material_cost
///
/// Where:
/// - production_cost = levelized_fixed_costs/units_produced_annually +
/// variable_cost_per_unit + (initial_investment/units_produced_annually) *
/// [property_tax_insurance_rate + (1/(1-income_tax_rate)) *
/// PMT(facility_lifetime, tax_modified_rate_of_return, 1, 0) -
/// (1/facility_lifetime) * (income_tax_rate/(1-income_tax_rate))]
/// - material_cost = Unit Cost of Material (weighted average of input material
/// unit values)
/// - units_produced_annually = production_capacity * timesteps_per_year
/// - levelized_fixed_costs = annual_fixed_costs
/// - variable_cost_per_unit = variable_cost_per_unit (provided directly by user)
/// - initial_investment = capital_cost
/// - property_and_insurance_rate = property_tax_rate + property_insurance_rate
/// - tax_modified_rate_of_return = WACC (weighted average cost of capital).
///   If discount_rate_override > 0 at institution level, uses that value.
///   Otherwise calculated as: (1-income_tax_rate)*bond_rate*bond_fraction +
///   shareholder_rate*shareholder_fraction
/// - income_tax_rate = Income Tax Rate (from institution)
/// - facility_lifetime = facility_depreciation_lifetime
///
/// The model assumes straight-line depreciation over the facility lifetime.
///
/// @param production_capacity Maximum throughput per timestep
/// @param input_cost_per_unit (Optional) per-unit cost of input materials used in the batch
/// @return Estimated levelized cost to produce one unit

double CalculateUnitCost(double production_capacity,
                         double input_cost_per_unit = 0.0) const {
  // Check if there's a cost override, and if so, use that
  if (cost_override > 0) {
    return cost_override + input_cost_per_unit;
  }

  // Get facility-level parameters
  double initial_investment;
  double facility_lifetime;
  double property_insurance_rate;
  double levelized_fixed_costs;
  double variable_cost_per_unit;
  if (!GetFacilityEconParameters(initial_investment, facility_lifetime,
                                  property_insurance_rate,
                                  levelized_fixed_costs,
                                  variable_cost_per_unit)) {
    return kDefaultUnitCost;
  }

  // Get institution-level parameters
  double income_tax_rate;
  double bond_rate;
  double bond_fraction;
  double shareholder_rate;
  double shareholder_fraction;
  double discount_rate_override;
  if (!GetInstitutionEconParameters(income_tax_rate, bond_rate, bond_fraction,
                                     shareholder_rate, shareholder_fraction,
                                     discount_rate_override)) {
    return kDefaultUnitCost;
  }

  // Get region-level parameters
  double property_tax_rate;
  if (!GetRegionEconParameters(property_tax_rate)) {
    return kDefaultUnitCost;
  }

  // Combine property tax (from region) and property insurance (from facility)
  double property_and_insurance_rate = property_tax_rate + property_insurance_rate;

  // Calculate intermediate values
  double units_produced_annually =
      CalcUnitsProducedAnnually(production_capacity);
  
  // Use discount_rate_override if provided, otherwise calculate manually
  double tax_modified_rate_of_return;
  if (discount_rate_override > 0) {
    tax_modified_rate_of_return = discount_rate_override;
  } else {
    tax_modified_rate_of_return = CalcTaxModifiedRateOfReturn(
        income_tax_rate, bond_rate, bond_fraction, shareholder_rate,
        shareholder_fraction);
  }

  // Calculate cost components
  double fixed_cost_per_unit =
      CalcFixedCostPerUnit(levelized_fixed_costs, units_produced_annually);
  double capital_cost_per_unit = CalcCapitalCostPerUnit(
      initial_investment, units_produced_annually, facility_lifetime,
      income_tax_rate, tax_modified_rate_of_return,
      property_and_insurance_rate);


  // Assemble total unit cost
  double production_cost = fixed_cost_per_unit + variable_cost_per_unit 
                            + capital_cost_per_unit;
  double unit_cost = production_cost + input_cost_per_unit;

  // Protects against divide by zero in pref = 1/unit_cost
  return unit_cost != 0 ? unit_cost : kDefaultUnitCost;
}

double CalculateUnitPrice(double production_capacity,
                          double input_cost_per_unit = 0.0) const {
  // Default implementation
  return CalculateUnitCost(production_capacity, input_cost_per_unit);
}

// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_capital_cost = {0};
std::vector<int> cycpp_shape_annual_fixed_costs = {0};
std::vector<int> cycpp_shape_facility_operational_lifetime = {0};
std::vector<int> cycpp_shape_facility_depreciation_lifetime = {0};
std::vector<int> cycpp_shape_variable_cost_per_unit = {0};
std::vector<int> cycpp_shape_cost_override = {0};
std::vector<int> cycpp_shape_property_insurance_rate = {0};
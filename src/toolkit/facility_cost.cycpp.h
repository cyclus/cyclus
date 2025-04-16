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
  std::unordered_map<std::string, double> econ_params{
      {"capital_cost", capital_cost},
      {"operations_and_maintenance", operations_and_maintenance},
      {"facility_lifetime", facility_lifetime},
      {"capacity_decline_factor", capacity_decline_factor},
      {"per_unit_labor_cost", per_unit_labor_cost}};

  return econ_params;
}

// Add the financial parameters to the class
void InitializeCosts() {
  std::unordered_map<std::string, double> econ_params = InitializeParmList();
  for (const auto& parameter : econ_params) {
    this->SetEconParameter(parameter.first, parameter.second);
  }
}

// Helper functions for GetCost
double DiscountedThroughputSum(double xt, double gamma, int T, double k) const {
  double sum = 0.0;
  for (int t = 1; t <= T; ++t) {
    sum += std::pow(xt, t) * std::pow(gamma, t);
  }
  return k * sum;
}

double DiscountedFixedCostSum(double F, double gamma, int T) const {
  double sum = 0.0;
  for (int t = 1; t <= T; ++t) {
    sum += F * std::pow(gamma, t);
  }
  return sum;
}

double DiscountedLaborCostSum(double labor_cost, double xt, double gamma, int T, double k) const {
  double sum = 0.0;
  for (int t = 1; t <= T; ++t) {
    sum += labor_cost * k * std::pow(xt, t) * std::pow(gamma, t);
  }
  return sum;
}

double ComputeDelta(double depreciation_constant, int initial_book_value,
                              int T_hat, double gamma, double alpha) const {
  std::vector<int> book_value = {initial_book_value};
  std::vector<double> dt;

  for (int i = 0; i < T_hat - 1; ++i) {
    int current_depreciation = static_cast<int>(-depreciation_constant * book_value[i]);
    book_value.push_back(book_value[i] + current_depreciation);
    dt.push_back(-static_cast<double>(current_depreciation) / book_value[0]);
  }

  double delta_partial_sum = 0.0;
  for (int t = 1; t < T_hat; ++t) {
    delta_partial_sum += dt[t - 1] * std::pow(gamma, t); 
  }

  return (1 - alpha * delta_partial_sum) / (1 - alpha);
}

double GetCost() {
  // Economic Parameters
  double r = parent()->GetEconParameter("minimum_acceptable_return_rate");
  double xt = GetEconParameter("capacity_decline_factor");
  double v = GetEconParameter("capital_cost");
  int T = static_cast<int>(GetEconParameter("facility_lifetime"));
  double OM = GetEconParameter("operations_and_maintenance");
  double labor_cost = GetEconParameter("per_unit_labor_cost");
  double alpha = parent()->GetEconParameter("corporate_income_tax_rate");
  double p = parent()->parent()->GetEconParameter("property_tax_rate");
  double depreciation_constant = parent()->GetEconParameter("depreciation_constant");
  int T_hat = T; // May become user-specified later

  double k = throughput * context()->dt();  // production scale factor
  double discount_factor = 1.0 / (1.0 + r);

  double property_tax = p * v;
  double F = OM + property_tax;
  double L = DiscountedThroughputSum(xt, discount_factor, T, k);

  double c = v / L;
  double f = DiscountedFixedCostSum(F, discount_factor, T) / L;
  double w = DiscountedLaborCostSum(labor_cost, xt, discount_factor, T, k);
  double delta = ComputeDelta(depreciation_constant, static_cast<int>(v), T_hat, discount_factor, alpha);

  std::cout << w + f + c * delta << std::endl;
  return w + f + c * delta;
}

// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_capital_cost = {0};
std::vector<int> cycpp_shape_operations_and_maintenance = {0};
std::vector<int> cycpp_shape_facility_lifetime = {0};
std::vector<int> cycpp_shape_capacity_decline_factor = {0};
std::vector<int> cycpp_shape_per_unit_labor_cost = {0};
std::vector<int> cycpp_shape_annual_labor_cost_increase_factor = {0};
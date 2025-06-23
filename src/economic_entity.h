#ifndef ECONOMIC_ENTITY_H
#define ECONOMIC_ENTITY_H

#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include "economic_parameter.h"

class EconomicEntity {
 public:

  /// @brief Fetches an economic parameter from the financial_data_ vector
  /// @param name the name of the parameter to fetch. Must be an exact match
  /// @return The value associated with the entry with "name" in financial_data_ 
  virtual double GetEconParameter(const std::string& name) const {
    for (auto& p : financial_data_) {
      if (p.name == name) {
        return p.value;
      }
    }
    throw std::out_of_range("Parameter not found: " + name);
  }

  /// @brief Add a new Economic parameter to financial_data_
  /// @param name The name of the EconParameter
  /// @param value The value of the EconParameter
  /// @param category The CostCategory of the EconParameter
  virtual void AddEconParameter(const std::string& name, double value, 
                                CostCategory category) {
    financial_data_.push_back({name, value, category});
  }

  /// @brief Get all EconParameters in a certain category
  /// @param cat The desired CostCategory
  /// @return A vector of all visible EconParameters for which category == cat 
  std::vector<EconParameter> GetByCategory(CostCategory cat) const {
    std::vector<EconParameter> out;
    for (auto& p : financial_data_) {
      if (p.category == cat) {
        out.push_back(p);
      }
    }
    return out;
  }

  /// @brief Sums the values of all EconParams in a CostCategory
  /// @param cat the CostCategory to sum the values over
  /// @return the sum of all values p.value in the CostCategory
  double SumByCategory(CostCategory cat) const {
    double total = 0.0;
    for (auto& p : GetByCategory(cat)) {
        total += p.value;
    }

    return total;
  }

  // Given default implementation so as not to break backwards compatability
  virtual std::vector<EconParameter> GenerateParamList() const {
    return {};
  }

  /// @brief Initialize the list of EconParameters from the ParamList
  void InitEconParameters() {
    std::vector<EconParameter> econ_params = GenerateParamList();
    for (const auto& parameter : econ_params) {
      AddEconParameter(parameter.name, parameter.value, parameter.category);
    }
  }

  /// @brief calculates the present value of a series of payments, using 
  /// discount rate i per time period, for future payment F that occurs n time 
  /// periods in the future plus regular payments A that occurs each time 
  /// period for n periods
  /// @param n Number of periods
  /// @param i Discout rate
  /// @param F Future payment
  /// @param A Regular payments at each period
  /// @return  Present value

  virtual double PV(int n, double i, double F, double A) const {
    
    double pv_F = F / std::pow((1 + i),n);
    double pv_A;
    
    // Since we're allowing certain terms to be zero
    if (i != 0.0) {
      pv_A = A * (1 - std::pow((1 + i), -n)) / i;
    } else {
      pv_A = A * n;
    }

    return pv_F + pv_A;
  }

  /// @brief calculates the future value of a series of payments, using 
  /// compound rate i per time period, for present payment P plus regular 
  /// payments A that occurs every period for n periods.
  /// @param n Number of periods
  /// @param i Compound Rate
  /// @param P Present payment
  /// @param A Regular payment at each period.
  /// @return Future value

  virtual double FV(int n, double i, double P, double A) const {

    double fv_P = P * std::pow((1 + i), n);
    double fv_A;

    // Since we're allowing certain terms to be zero
    if (i != 0.0) {
        fv_A = A * (std::pow((1 + i), n) - 1) / i;
    } else {
        fv_A = A * n;
    }

    return fv_P + fv_A;
  }

  /// @brief computes the regular payment for n time periods that is equivalent 
  /// to the combination of a single payment of P immediately and a single 
  /// payment F after n time periods, using a discount rate of i per time periods
  /// @param n Number of payments
  /// @param i Discount rate
  /// @param P Immediate payment 
  /// @param F Future payment
  /// @return the regular payment (PMT) which is equivalent to P and F

  virtual double PMT(int n, double i, double P, double F) const {

    if (n <= 0) {
      throw std::invalid_argument("n must be greater than or equal to zero!");
    }

    double p_term = P;
    double f_term = F / std::pow((1 + i), n);
    
    // Since we're allowing certain terms to be zero
    if (i != 0) {
        return (p_term + f_term) * i / (1 - std::pow((1 + i), -n));
    } else {
        return (p_term + f_term) / n;
    }
  }

  /// @brief calculates the present value of a series of payments described by 
  /// the vector [A], occurring over a time period defined by the length of [A], 
  /// with a discount rate of i per time periods
  /// @param i Discount rate
  /// @param A Vector of payments which also defines how many time periods are
  /// summed over (by the length of A)
  /// @return Present value

  virtual double PV(double i, const std::vector<double>& A) const {
      double pv = 0.0;

      for (int t = 0; t < A.size(); ++t) {
          pv += A[t] / std::pow((1 + i), t+1);
      }

      return pv;
  }

  private:
  std::vector<EconParameter> financial_data_;
};

#endif  // ECONOMIC_ENTITY_H

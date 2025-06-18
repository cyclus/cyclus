#ifndef ECONOMIC_ENTITY_H
#define ECONOMIC_ENTITY_H

#include <stdexcept>
#include <string>
#include <unordered_map>

class EconomicEntity {
 public:
  virtual double GetEconParameter(const std::string& key) const {
    auto it = financial_data_.find(key);
    if (it != financial_data_.end()) {
      return it->second;
    } else {
      throw std::runtime_error("Key '" + key +
                               "' not found in financial_data_");
    }
  }
  virtual void SetEconParameter(const std::string& key, double value) {
    financial_data_[key] = value;
  }

  // Given default implementation so as not to break backwards compatability
  virtual std::unordered_map<std::string, double> GenerateParamList() const {
    return {};
  };

  void InitEconParameters() {
    std::unordered_map<std::string, double> econ_params = GenerateParamList();
    for (const auto& parameter : econ_params) {
      SetEconParameter(parameter.first, parameter.second);
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
  /// @param n Number of periods
  /// @param i Discount rate
  /// @param P Immediate payment 
  /// @param F Future payment
  /// @return the regular payment (PMT) which is equivalent to P and F

  virtual double PMT(int n, double i, double P, double F) const {

    double p_term = P;
    double f_term = F / std::pow((1 + i), n);
    
    // Since we're allowing certain terms to be zero
    if (i != 0) {
        return (p_term + f_term) * i / (1 - std::pow((1 + i), -n));
    } else {
        return (p_term + F) / n;
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
  std::unordered_map<std::string, double> financial_data_;
};

#endif  // ECONOMIC_ENTITY_H

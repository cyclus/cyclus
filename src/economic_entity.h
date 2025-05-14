#ifndef ECONOMIC_ENTITY_H
#define ECONOMIC_ENTITY_H

#include <stdexcept>
#include <string>
#include <unordered_map>

class EconomicEntity {
 public:
  virtual double GetEconParameter(const std::string& key) const {
    return GetParameter(key);
  }
  virtual void SetEconParameter(const std::string& key, double value) {
    SetParameter(key, value);
  }

  // Given default implementation so as not to break backwards compatability
  virtual std::unordered_map<std::string, double> InitializeParamList() const {
    return {};
  };

  void InitializeCosts() {
    std::unordered_map<std::string, double> econ_params = InitializeParamList();
    for (const auto& parameter : econ_params) {
      this->SetEconParameter(parameter.first, parameter.second);
    }
  }

  double ComputePresentValue(double r, int T, double cdf = 1.0) const {
    if (T <= 0) {
      return 0.0;
    }

    // Protects against divide by zero in discount factor
    if (std::abs(r + 1.0) < 1e-12) {
      return static_cast<double>(T);
    }

    double discount_factor = 1.0 / (1.0 + r);
    double modified_discount_factor = discount_factor * cdf;

    // Protects against divide by zero in the geometric series
    if (std::abs(modified_discount_factor - 1.0) < 1e-12) {
      return static_cast<double>(T);
    }

    // The geometric series representation of the sum
    return modified_discount_factor *
           (1.0 - std::pow(modified_discount_factor, T)) /
           (1.0 - modified_discount_factor);
  }

 protected:
  void SetParameter(const std::string& key, double value) {
    financial_data_[key] = value;
  }

  double GetParameter(const std::string& key) const {
    auto it = financial_data_.find(key);
    if (it != financial_data_.end()) {
      return it->second;
    } else {
      throw std::runtime_error("Key '" + key +
                               "' not found in financial_data_");
    }
  }

 private:
  std::unordered_map<std::string, double> financial_data_;
};

#endif  // ECONOMIC_ENTITY_H

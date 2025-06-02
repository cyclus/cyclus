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

    // This allows us to test more easily
    this->SetEconParameter("UnitTestHook", -1.0);
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

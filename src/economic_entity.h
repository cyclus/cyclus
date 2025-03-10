#ifndef ECONOMIC_ENTITY_H
#define ECONOMIC_ENTITY_H

#include <string>
#include <unordered_map>
#include <stdexcept>

class EconomicEntity {
 public:
 virtual double GetEconParameter(const std::string& key) const = 0;
 virtual void SetEconParameter(const std::string& key, double value) = 0;

 protected:
  void SetParameter(const std::string& key, double value) {
    financial_data_[key] = value;
  }

  double GetParameter(const std::string& key) const {
    auto it = financial_data_.find(key);
    if (it != financial_data_.end()) {
      return it->second;
    } else {
      throw std::runtime_error("Key '" + key + "' not found in financial_data_");
    }
  }

 private:
  std::unordered_map<std::string, double> financial_data_;
};

#endif // ECONOMIC_ENTITY_H

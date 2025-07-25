#ifndef CYCLUS_SRC_TOOLKIT_COMMODITY_H_
#define CYCLUS_SRC_TOOLKIT_COMMODITY_H_

#include <string>

namespace cyclus {
namespace toolkit {
/// A simple class defining a commodity; it is currently super simple.
/// The reason this class exists is so that code may be cleaner and more
/// straightforward while one could have chosen to typedef a string, there may
/// be some reason to extend the class in the future.
class Commodity {
 public:
  /// Default constructor
  Commodity();

  /// Constructor
  /// @param name the name of the commodity
  Commodity(std::string name);

  /// The commodity's name
  std::string name() const;

  /// Equality operator
  bool operator==(const Commodity& other) const;

  /// Inequality operator
  bool operator!=(const Commodity& other) const;

 private:
  /// The name of the commodity
  std::string name_;
};

/// A comparator so that commodities may be used in maps
/// we do not care how they are compared, only that they can be
struct CommodityCompare {
  inline bool operator()(const Commodity& lhs, const Commodity& rhs) const {
    return lhs.name() < rhs.name();
  }
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_COMMODITY_H_

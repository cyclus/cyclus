#ifndef CYCLUS_SRC_TOOLKIT_FACILITY_COST_H_
#define CYCLUS_SRC_TOOLKIT_FACILITY_COST_H_


#include <string>
#include "cyclus.h"

namespace cyclus {
namespace toolkit {

/// @class FacilityCost
/// The FacilityCost class is a class that handles basic cost data for
/// Facilities within cyclus. The inner workings of this class are TBD,
/// but it will first serve as a very basic showcase of code injection into
/// Facility header files for state variables which can be common to all
/// Facilities.

class FacilityCost {
 public:

  /// The default constructor for FacilityCost. This creates an object
  /// with all costs set to 0.0 as default, but with the option to set
  /// them manually.
  FacilityCost(double capital_cost = 0) : capital_cost_(capital_cost) {}

  /// The default destructor for FacilityCost
  ~FacilityCost() = default;

  // A function to list all costs as strings
  std::string EnumerateCosts();

  // Setter for capital_cost_
  void setCapitalCost(double capital_cost) {
    capital_cost_ = capital_cost;
  }

 private:
  /// Capital cost of the facility
  double capital_cost_;

};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_POSITION_H_
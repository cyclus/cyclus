#include "facility_cost.h"

#include <math.h>
#include <stdio.h>
#include <iomanip>
#include <sstream>

namespace cyclus {
namespace toolkit {

// Interested in thoughts on the member initializer list
FacilityCost::FacilityCost() : capital_cost_(0) {}

FacilityCost::FacilityCost(double capital_cost) {
  capital_cost_ = capital_cost;
}

FacilityCost::~FacilityCost() {}

std::string FacilityCost::EnumerateCosts(){
    std::ostringstream strs;
    strs << capital_cost_;
    std::string costs = strs.str();
    return costs;
}

}  // namespace toolkit
}  // namespace cyclus
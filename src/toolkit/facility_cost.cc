#include "facility_cost.h"

#include <math.h>
#include <stdio.h>
#include <iomanip>
#include <sstream>

namespace cyclus {
namespace toolkit {

std::string FacilityCost::EnumerateCosts(){
    std::ostringstream strs;
    strs << capital_cost_;
    std::string costs = strs.str();
    return costs;
}

}  // namespace toolkit
}  // namespace cyclus
/// This includes the required header to add facility costs to archetypes.
/// One should only need to:
/// - '#include "toolkit/facility_cost.cycpp.h"' in the header of the 
///    archetype class (as private)
/// - '#include "toolkit/facility_cost.h"' in the main include section
///    of the header of the archetype class.

cyclus::toolkit::FacilityCost cost;

#pragma cyclus var { \
"default": 0.0, \
"uilabel": "Capital cost required to build facility", \
"doc": "Capital cost required to build facility" \
}
double capital_cost;

// Required for compilation but not added by the cycpp preprocessor. Do not
// remove.
 std::vector<int> cycpp_shape_capital_cost = {0};
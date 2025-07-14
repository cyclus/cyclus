/// This includes the required header to add a material sell policy to archetypes.
/// One should only need to:
/// - '#include "toolkit/matl_sell_policy.cycpp.h"' in the header of the
///    archetype class (it is strongly recommended to inject this snippet as 
///    `private:`, but archetype developers are free to make other choices)

/// How to add parameters to this file:
/// 1. Add the pragma. A default value MUST be added to ensure backwards
///    compatibility.
/// 2. Add "std::vector<int> cycpp_shape_<param_name> = {0};" to the end of the
///    file with the other ones, reaplcing <param_name> with the name you put
///    in the econ_params array (again, must match exactly).

/// @addtogroup optionalparams
/// @{
/// sell_quantity restricts selling to only integer multiples of this value
/// @}

// clang-format off
#pragma cyclus var {"default": 0,\
                    "tooltip":"sell quantity (kg)",\
                    "doc":"material will be sold in integer multiples of this quantity. If"\
                    " the buffer contains less than the sell quantity, no material will be"\
                    " offered", \
                    "uilabel":"Sell Quantity",\
                    "uitype": "range", \
                    "range": [0.0, CY_LARGE_DOUBLE], \
                    "units": "kg"}
double sell_quantity;
// clang-format on

//// A policy for sending material
cyclus::toolkit::MatlSellPolicy sell_policy;

// Required for compilation but not added by the cycpp preprocessor. Do not
// remove. Must be one for each variable.
std::vector<int> cycpp_shape_sell_quantity = {0};
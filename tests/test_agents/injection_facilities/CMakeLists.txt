# one call per injectable test facility. Tests go beside these in the folder
use_cyclus("InjectionTestAgents" "unit_cost_test_facility")

# This only needs to be done once, and needs to be after all use_cyclus calls
install_cyclus_module("InjectionTestAgents" "tests" "NONE")

set(INJECTION_FACILITIES_TEST_SOURCE
    "${InjectionTestAgents_TEST_CC}"
    PARENT_SCOPE)
set(INJECTION_FACILITIES_TEST_LIBRARY
    "InjectionTestAgents"
    PARENT_SCOPE)

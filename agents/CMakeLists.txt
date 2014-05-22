SET(INCL_DIRS "${CYCLUS_CORE_INCLUDE_DIRS}" "${CYCLUS_TEST_INCLUDE_DIRS}")
INCLUDE_DIRECTORIES(${INCL_DIRS})

use_cyclus("coreagents" "null_inst")
use_cyclus("coreagents" "null_region")
use_cyclus("coreagents" "sink")
use_cyclus("coreagents" "source")
use_cyclus("coreagents" "k_facility")
use_cyclus("coreagents" "prey")
use_cyclus("coreagents" "predator")
install_cyclus_module("coreagents" "" "NONE")

# # Build cycamore_unit_tests
# SET(TGT cyclus_agent_unit_tests)
# ADD_EXECUTABLE( 
#   ${TGT}
#   cyclus_agent_unit_test_driver.cc
#   ${coreagents_TEST_CC}
# )

# TARGET_LINK_LIBRARIES( 
#   ${TGT} dl
#   ${coreagents_TEST_LIB}
#   ${LIBS} 
#   cyclus
#   gtest
#   )

# INSTALL(
#   TARGETS ${TGT}
#   RUNTIME DESTINATION bin
#   COMPONENT testing
#   )

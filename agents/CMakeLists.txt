SET(INCL_DIRS "${CYCLUS_CORE_INCLUDE_DIRS}" "${CYCLUS_TEST_INCLUDE_DIRS}")
INCLUDE_DIRECTORIES(${INCL_DIRS})

use_cyclus("agents" "null_inst")
use_cyclus("agents" "null_region")
use_cyclus("agents" "sink")
use_cyclus("agents" "source")
use_cyclus("agents" "k_facility")
use_cyclus("agents" "prey")
use_cyclus("agents" "predator")
install_cyclus_module("agents" "")

SET(
  AGENT_TST_SRC
  ${NullInst_TEST_CC}
  ${NullRegion_TEST_CC}
  ${Sink_TEST_CC}
  ${Source_TEST_CC}
  ${KFacility_TEST_CC}
  ${Prey_TEST_CC}
  ${Predator_TEST_CC}
  PARENT_SCOPE
  )

SET(TGT cyclus_agent_unit_tests)

# Build cycamore_unit_tests
ADD_EXECUTABLE( 
  ${TGT}
  cyclus_agent_unit_test_driver.cc 
  ${AGENT_TST_SRC}
)

TARGET_LINK_LIBRARIES( 
  ${TGT} dl
  ${LIBS} 
  cyclus
  gtest
  )

INSTALL(
  TARGETS ${TGT}
  RUNTIME DESTINATION bin
  COMPONENT testing
  )

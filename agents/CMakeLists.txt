INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS})

use_cyclus("agents" "null_inst")
use_cyclus("agents" "null_region")
use_cyclus("agents" "sink")
use_cyclus("agents" "source")
use_cyclus("agents" "k_facility")
use_cyclus("agents" "prey")
use_cyclus("agents" "predator")
install_cyclus_module("agents" "" "NONE")

SET(
  CYCLUS_CORE_TEST_SOURCE "${CYCLUS_CORE_TEST_SOURCE}"
  "${agents_TEST_CC}"
  PARENT_SCOPE
  )
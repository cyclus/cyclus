INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS} ${CYCLUS_CORE_TEST_INCLUDE_DIRS})

use_cyclus("agents" "null_inst")
use_cyclus("agents" "null_region")
install_cyclus_module("agents")

#install_cyclus_standalone("NullInst" "null_inst")
#install_cyclus_standalone("NullRegion" "null_region")
#install_cyclus_standalone("Sink" "sink")
#install_cyclus_standalone("Source" "source")
#install_cyclus_standalone("KFacility" "k_facility")
#install_cyclus_standalone("Prey" "prey")
#install_cyclus_standalone("Predator" "predator")

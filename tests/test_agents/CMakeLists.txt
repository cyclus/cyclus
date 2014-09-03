INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS})

install_cyclus_standalone("TestAgent" "test_agent" "tests")
install_cyclus_standalone("TestFacility" "test_facility" "tests")
install_cyclus_standalone("TestInst" "test_inst" "tests")
install_cyclus_standalone("TestRegion" "test_region" "tests")

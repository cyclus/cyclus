
INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS} ${CYCLUS_TEST_INCLUDE_DIRS})
install_cyclus_standalone("StubFacility" "stub_facility" "stubs")
install_cyclus_standalone("StubInst" "stub_inst" "stubs")
install_cyclus_standalone("StubRegion" "stub_region" "stubs")

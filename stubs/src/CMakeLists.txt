INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS} ${CYCLUS_CORE_TEST_INCLUDE_DIR})
install_cyclus_standalone("StubFacility" "stub_facility" "stubs")
install_cyclus_standalone("StubInst" "stub_inst" "stubs")
install_cyclus_standalone("StubRegion" "stub_region" "stubs")
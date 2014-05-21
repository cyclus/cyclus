
INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS} ${CYCLUS_TEST_INCLUDE_DIRS})
# MESSAGE("cyclus_core incls: ${CYCLUS_CORE_INCLUDE_DIRS}")
# MESSAGE("cyclus_core_test incls: ${CYCLUS_CORE_TEST_INCLUDE_DIRS}")
install_cyclus_standalone("StubFacility" "stub_facility" "stubs")
install_cyclus_standalone("StubInst" "stub_inst" "stubs")
install_cyclus_standalone("StubRegion" "stub_region" "stubs")

SET(
  TST_SRC
  ${StubFacility_TEST_CC}
  ${StubInst_TEST_CC}
  ${StubRegion_TEST_CC}
  )

SET(TGT cyclus_stub_unit_tests)

# Build cycamore_unit_tests
ADD_EXECUTABLE( 
  ${TGT}
  cyclus_stub_unit_test_driver.cc 
  ${TST_SRC}
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

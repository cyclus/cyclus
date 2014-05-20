
ADD_EXECUTABLE( 
  cyclus_unit_tests
  ${CYCLUS_CORE_TEST_SOURCE}
  cyclus_unit_test_driver.cc 
  )

TARGET_LINK_LIBRARIES( 
  cyclus_unit_tests dl 
  ${LIBS} 
  cyclus
  gtest
  )

INSTALL(
  TARGETS cyclus_unit_tests
  RUNTIME DESTINATION bin
  COMPONENT testing
  )

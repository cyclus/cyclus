##############################################################################################
##################################### begin cyclus app #######################################
##############################################################################################

INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS})

# Build the cyclus executable from the CYCLUS_SOURCE source files
SET( CYCLUS_SOURCE ${CYCLUS_SOURCE}
  cyclus.cc
  )

ADD_EXECUTABLE( cyclus_cli ${CYCLUS_SOURCE} )
set_target_properties(cyclus_cli PROPERTIES OUTPUT_NAME cyclus)

TARGET_LINK_LIBRARIES( cyclus_cli dl ${LIBS} cyclus )

INSTALL(
  TARGETS cyclus_cli
  RUNTIME DESTINATION bin
  COMPONENT cyclus
  )

INSTALL(
  PROGRAMS cycpp.py
  DESTINATION bin
  COMPONENT core
)

##############################################################################################
###################################### end cyclus app ########################################
##############################################################################################

##############################################################################################
################################## begin cyclus unit tests ###################################
##############################################################################################

ADD_EXECUTABLE( 
  cyclus_unit_tests
  ${CYCLUS_CORE_TEST_SOURCE}
  cyclus_unit_test_driver.cc 
  )

TARGET_LINK_LIBRARIES( 
  cyclus_unit_tests dl 
  ${LIBS} 
  ${CYCLUS_TEST_LIBRARIES}
  )

INSTALL(
  TARGETS cyclus_unit_tests
  RUNTIME DESTINATION bin
  COMPONENT testing
  )

##############################################################################################
################################### end cyclus unit tests ####################################
##############################################################################################

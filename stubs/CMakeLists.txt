
##############################################################################################
#################################### begin cyclus stubs ######################################
##############################################################################################

INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS})

SET(CYCLUS_STUB_SOURCE ${CYCLUS_STUB_SOURCE}
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_facility.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_inst.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_region.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_market.cc
  )

ADD_LIBRARY( cyclusstubs ${CYCLUS_STUB_SOURCE} )

SET_TARGET_PROPERTIES( cyclusstubs
  PROPERTIES 
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
  )

TARGET_LINK_LIBRARIES( cyclusstubs dl 
  ${LIBS} 
  cycluscore 
  )

INSTALL(
  TARGETS cyclusstubs
  LIBRARY DESTINATION lib
  COMPONENT cycluscore
  )

# add stub test source to be used by tests
SET( CYCLUS_STUB_TEST_SOURCE ${CYCLUS_STUB_TEST_SOURCE}
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_facility_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_inst_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_region_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_market_tests.cc
  PARENT_SCOPE
  )

##############################################################################################
##################################### end cyclus stubs #######################################
##############################################################################################


# add stub source to core source
SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC}
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_facility.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_inst.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_region.cc
  # ${CMAKE_CURRENT_SOURCE_DIR}/stub_market.cc
  PARENT_SCOPE)

# add stub test source to test source
SET( CYCLUS_TEST_CORE ${CYCLUS_TEST_CORE}
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_facility_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_inst_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_region_tests.cc
  # ${CMAKE_CURRENT_SOURCE_DIR}/stub_market_tests.cc
  PARENT_SCOPE)

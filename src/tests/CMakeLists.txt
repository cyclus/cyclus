# Find tests generated in the binary directory
message("GENERATED TESTS DIR ${CMAKE_CURRENT_BINARY_DIR}")
message("GENERATED TESTS ${GENERATED_FILTER_LIST}")

ADD_SUBDIRECTORY(cyclopts)
ADD_SUBDIRECTORY(Models)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/Models
  )

SET( TestSource ${TestSource} PARENT_SCOPE )

# To add a new file, just add it to this list.  Any GoogleTests inside will be automatically
# added to ctest.
set ( CYCLUS_TEST_CORE 
  ${CYCLUS_TEST_CORE}
  # --
  ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_loading_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/building_manager_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/building_test_helper.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/commodity_producer_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/commodity_producer_manager_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/commodity_test_helper.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/comp_math_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/composition_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/csv_back_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/cyc_arithmetic_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/enrichment_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/environment_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/event_manager_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/facility_model_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/inst_model_class_tests.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/market_player_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/mass_table_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/mat_query_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/material_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/message_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/model_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/region_model_class_tests.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/resource_buff_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/sd_manager_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/sqlite_db_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/sqlite_back_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/symbolic_function_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/xml_file_loader_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/xml_query_engine_tests.cc
  PARENT_SCOPE)

INSTALL(FILES GoogleTest/gtest/gtest.h
  DESTINATION include/gtest
  COMPONENT testing
  )

FILE(GLOB header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB model_test_files "${CMAKE_CURRENT_SOURCE_DIR}/*model_tests.cc")
INSTALL(FILES ${header_files} ${model_test_files}
  DESTINATION include/cyclus
  COMPONENT testing
  )

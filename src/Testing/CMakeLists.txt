# Find tests generated in the binary directory
message("GENERATED TESTS DIR ${CMAKE_CURRENT_BINARY_DIR}")
message("GENERATED TESTS ${GENERATED_FILTER_LIST}")

ADD_SUBDIRECTORY(cyclopts)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/inst_model_class_tests.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/market_player_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/mass_table_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/mat_query_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/material_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/message_tests.cc
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

# Add all the tests by parsing the source code
# This macro searches for GoogleTest macros and adds them as tests automatically
macro(ADD_GOOGLE_TESTS executable)
  # Add the generated tests
  set ( AllTestsHits ${GENERATED_TEST_LIST} )

  foreach ( source ${ARGN} )
    file(READ "${source}" contents)

    # Find all test lists
    string(REGEX MATCHALL "TEST_?[FP]?\\(([A-Za-z_0-9 ,_]+)\\)" AllTests ${contents})

    # Convert the C++ code into a short test name
    foreach(hit ${AllTests})
      string(REGEX REPLACE ".*\\(( *[A-Za-z_0-9]+)[, ]*([A-Za-z_0-9]+) *\\).*" "\\1.\\2" test_name ${hit})
      set ( AllTestsHits ${AllTestsHits} ${test_name} )
    endforeach()
  endforeach()

  list ( SORT AllTestsHits )
  # Add all the remaining tests to CTest's test list
  foreach(hit ${AllTestsHits})
    add_test(${hit} ${executable} --gtest_filter=${hit})
  endforeach()
endmacro()

INSTALL(FILES GoogleTest/gtest/gtest.h
  DESTINATION include/gtest
  COMPONENT testing
  )

FILE(GLOB header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB model_test_files "${CMAKE_CURRENT_SOURCE_DIR}/*model_tests.cc")
INSTALL(FILES ${header_files} ${model_test_files}
  DESTINATION include
  COMPONENT testing
  )

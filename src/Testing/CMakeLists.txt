# Find tests generated in the binary directory
message("GENERATED TESTS DIR ${CMAKE_CURRENT_BINARY_DIR}")
message("GENERATED TESTS ${GENERATED_FILTER_LIST}")

# To add a new file, just add it to this list.  Any GoogleTests inside will be automatically
# added to ctest.
set ( CYCLUS_TEST_CORE 
  # -- do not change this order
  ${CMAKE_CURRENT_SOURCE_DIR}/TableTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DatabaseTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/BookKeeperTests.cpp
  # --
  ${CMAKE_CURRENT_SOURCE_DIR}/BuildingManagerTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/BuildingTestHelper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CompMapTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CommodityProducerTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CommodityProducerManagerTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CommodityTestHelper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DecayHandlerTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/InstModelClassTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/IsoVectorTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MarketPlayerTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MassTableTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MaterialTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MessageTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/RegionModelClassTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/ResourceBuffTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SDManagerTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/XMLParserTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/XMLFileLoaderTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/XMLQueryEngineTests.cpp
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
  DESTINATION cyclus/include/gtest
  COMPONENT testing
  )

FILE(GLOB header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB model_test_files "${CMAKE_CURRENT_SOURCE_DIR}/*ModelTests.cpp")
INSTALL(FILES ${header_files} ${model_test_files} 
  DESTINATION cyclus/include
  COMPONENT testing
  )

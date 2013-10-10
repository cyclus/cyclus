##############################################################################################
#################################### begin google test #######################################
##############################################################################################

# Be sure to clear these each time
SET( GENERATED_TEST_LIST "" CACHE INTERNAL "" )
SET( GENERATED_FILTER_LIST "" CACHE INTERNAL "" )
SET( GENERATED_TEST_SOURCE_LIST "" CACHE INTERNAL "" )

# The gtest include file directories.
SET(GTEST_INCLUDE_DIRS 
  "${CMAKE_CURRENT_SOURCE_DIR}/GoogleTest"
  )

# The gtest library directories.
SET(GTEST_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}/lib")

# Alerts CMake to any tests we need to worry about
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

# Create an executable for all the gtests
# Options for testing
OPTION( USE_TESTING "Build testing" ON )
IF( USE_TESTING )
  enable_testing()
  INCLUDE(CTest)
ENDIF()

# ADD_SUBDIRECTORY(tests)
# SET(TestSource 
#   ${TestSource} 
#   ${CYCLUS_TEST_CORE}
#   )

FIND_PACKAGE(Threads)
IF(CMAKE_USE_PTHREADS_INIT)  # The pthreads library is available.
  SET(cxx_base_flags "${cxx_base_flags} -DGTEST_HAS_PTHREAD=1")
ENDIF()

# Build libgtest
SET( GTest
  ${CMAKE_CURRENT_SOURCE_DIR}/GoogleTest/gtest/gtest-all.cc
  )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

ADD_LIBRARY(gtest ${GTest})

IF (CMAKE_USE_PTHREADS_INIT)
  TARGET_LINK_LIBRARIES(gtest ${CMAKE_THREAD_LIBS_INIT})
ENDIF()

SET_TARGET_PROPERTIES(gtest
  PROPERTIES INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
)

INSTALL(TARGETS gtest
  LIBRARY DESTINATION lib
  COMPONENT testing
  )

##############################################################################################
##################################### end google test ########################################
##############################################################################################

# # Build CyclusUnitTestDriver
# ADD_EXECUTABLE( CyclusUnitTestDriver
#   tests/cyclus_unit_test_driver.cc ${TestSource}
#   ${GENERATED_TEST_SOURCE}
# )
# TARGET_LINK_LIBRARIES( CyclusUnitTestDriver dl ${LIBS} cycluscore gtest)

# INSTALL(TARGETS CyclusUnitTestDriver
#   RUNTIME DESTINATION bin
#   COMPONENT testing
#   )

# # read tests after building the driver, and add them to ctest
# set( tgt "CyclusUnitTestDriver")
# set( script "${CYCLUS_SOURCE_DIR}/tests/generate_test_macros.py")
# set( exec "--executable=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${tgt}")
# set( out "--output=${CYCLUS_BINARY_DIR}/CTestTestfile.cmake")
# MESSAGE(${out})
# add_custom_command(TARGET ${tgt}
#   POST_BUILD
#   COMMAND python ${script} ${exec} ${out}
#   COMMENT "adding tests from ${tgt}"
#   DEPENDS
#   VERBATIM
#   )


# # Find tests generated in the binary directory
# message("GENERATED TESTS DIR ${CMAKE_CURRENT_BINARY_DIR}")
# message("GENERATED TESTS ${GENERATED_FILTER_LIST}")

# ADD_SUBDIRECTORY(cyclopts)
# ADD_SUBDIRECTORY(Models)

# INCLUDE_DIRECTORIES(
#   ${CMAKE_CURRENT_SOURCE_DIR}/Models
#   )

# SET( TestSource ${TestSource} PARENT_SCOPE )

# # To add a new file, just add it to this list.  Any GoogleTests inside will be automatically
# # added to ctest.
# set ( CYCLUS_TEST_CORE 
#   ${CYCLUS_TEST_CORE}
#   # --
#   ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_loading_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/building_manager_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/building_test_helper.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/commodity_producer_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/commodity_producer_manager_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/commodity_test_helper.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/comp_math_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/composition_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/csv_back_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/cyc_arithmetic_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/enrichment_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/environment_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/event_manager_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/facility_model_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/inst_model_class_tests.cc 
#   ${CMAKE_CURRENT_SOURCE_DIR}/market_player_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/mass_table_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/mat_query_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/material_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/message_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/model_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/region_model_class_tests.cc 
#   ${CMAKE_CURRENT_SOURCE_DIR}/resource_buff_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/sd_manager_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/sqlite_db_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/sqlite_back_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/symbolic_function_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/xml_file_loader_tests.cc
#   ${CMAKE_CURRENT_SOURCE_DIR}/xml_query_engine_tests.cc
#   PARENT_SCOPE)

# INSTALL(FILES GoogleTest/gtest/gtest.h
#   DESTINATION include/gtest
#   COMPONENT testing
#   )

# FILE(GLOB header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
# FILE(GLOB model_test_files "${CMAKE_CURRENT_SOURCE_DIR}/*model_tests.cc")
# INSTALL(FILES ${header_files} ${model_test_files}
#   DESTINATION include/cyclus
#   COMPONENT testing
#   )

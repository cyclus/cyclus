
# # Include the core header files in the app build manually.
# # Since we build the core library in the same project, this is safer.
# SET( CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR}
#   ${CYCLUS_SOURCE_DIR}
#   )

# # include all the directories we just found
# INCLUDE_DIRECTORIES( ${CYCLUS_INCLUDE_DIR} )

# # add stub directory; this positioning is required because the stubs have tests
# # defined, and thus must be incorporated before the test target is made
# ADD_SUBDIRECTORY(stubs)

# # ------------------------- Google Test -----------------------------------

# # Be sure to clear these each time
# SET( GENERATED_TEST_LIST "" CACHE INTERNAL "" )
# SET( GENERATED_FILTER_LIST "" CACHE INTERNAL "" )
# SET( GENERATED_TEST_SOURCE_LIST "" CACHE INTERNAL "" )

# # The gtest include file directories.
# SET(GTEST_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/tests/GoogleTest")

# # The gtest library directories.
# SET(GTEST_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}/lib")

# # Alerts CMake to any tests we need to worry about
# INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

# # Create an executable for all the gtests
# # Options for testing
# OPTION( USE_TESTING "Build testing" ON )
# IF( USE_TESTING )
#   enable_testing()
#   INCLUDE(CTest)
# ENDIF()

# ADD_SUBDIRECTORY(tests)
# SET(TestSource 
#   ${TestSource} 
#   ${CYCLUS_TEST_CORE}
#   )

# FIND_PACKAGE(Threads)
# IF(CMAKE_USE_PTHREADS_INIT)  # The pthreads library is available.
#   SET(cxx_base_flags "${cxx_base_flags} -DGTEST_HAS_PTHREAD=1")
# ENDIF()

# # Build libgtest
# SET( GTest
#   tests/GoogleTest/gtest/gtest-all.cc
# )
# INCLUDE_DIRECTORIES( tests )

# ADD_LIBRARY(gtest ${GTest})

# IF (CMAKE_USE_PTHREADS_INIT)
#   TARGET_LINK_LIBRARIES(gtest ${CMAKE_THREAD_LIBS_INIT})
# ENDIF()

# SET_TARGET_PROPERTIES(gtest
#   PROPERTIES INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
#   INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
# )

# INSTALL(TARGETS gtest
#   LIBRARY DESTINATION lib
#   COMPONENT testing
#   )

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

# SET(CPACK_COMPONENTS_ALL cyclus testing libraries data core)

# # Here's where we package it with CPack
# SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cyclus : A nuclear fuel cycle simulator from UW-Madison.")

# SET(CPACK_GENERATOR "DEB")
# SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "CNERG, UW-Madison") #required
# SET(CPACK_PACKAGE_VENDOR "CNERG, UW-Madison")
# SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../README.rst")
# SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE.rst")
# SET(CPACK_PACKAGE_VERSION_MAJOR "0")
# SET(CPACK_PACKAGE_VERSION_MINOR "2")
# #SET(CPACK_PACKAGE_VERSION_PATCH "2") # should use commit number here  ?
# SET(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
# SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY "1")
# SET(CPACK_PACKAGE_INSTALL_DIRECTORY "cyclus${CPACK_VERSION_MAJOR}.${CPACK_VERSION_MINOR}")
# #SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev  (>= 1.46.0-1), libxml++2.6-dev (>= 2.6.0), libsqlite3-dev (>= 3.7.9), libhdf5-serial-dev (>= 1.8.0)")
# set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
# CONFIGURE_FILE("${CYCLUS_SOURCE_DIR}/CMake/CyclusCPackOptions.cmake.in"
#   "${CYCLUS_BINARY_DIR}/CMake/CyclusCPackOptions.cmake" @ONLY)
# SET(CPACK_PROJECT_CONFIG_FILE "${CYCLUS_BINARY_DIR}/CMake/CyclusCPackOptions.cmake")
# SET(CPACK_PACKAGE_EXECUTABLES "cyclus" "CyclusUnitTestDriver")

# INCLUDE(CPack)

# ENDIF(NOT CYCLUS_DOC_ONLY)  

# # This is the directory that holds the doxygen doxyfile template (doxy.conf.in)
# SET( DOC_INPUT_DIR ${CYCLUS_SOURCE_DIR}/doc)
  
# # If doxygen exists, use the doc/CMakeLists.txt file for further instructions. 
# FIND_PACKAGE(Doxygen)
# IF (DOXYGEN_FOUND)
#   ADD_SUBDIRECTORY(doc)
#   SET( DOC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc )
# ELSE (DOXYGEN_FOUND)
#   MESSAGE(STATUS "WARNING: Doxygen not found - doc won't be created")
# ENDIF (DOXYGEN_FOUND)

#################################################################
############# special file configuration ########################
#################################################################

CONFIGURE_FILE(suffix.h.in ${CMAKE_CURRENT_SOURCE_DIR}/suffix.h @ONLY)

EXECUTE_PROCESS(COMMAND git describe OUTPUT_VARIABLE core_version OUTPUT_STRIP_TRAILING_WHITESPACE)
CONFIGURE_FILE(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h @ONLY)

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/mass.sqlite
  ${CYCLUS_BINARY_DIR}/share/mass.sqlite
  COPYONLY
  )

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/decayInfo.dat
  ${CYCLUS_BINARY_DIR}/share/decayInfo.dat
  COPYONLY
  )

SET(cyclus_install_dir ${CMAKE_INSTALL_PREFIX})
SET(cyclus_build_dir ${CYCLUS_BINARY_DIR})
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/env.cc.in
  ${CMAKE_CURRENT_SOURCE_DIR}/env.cc
  @ONLY
  )

INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/mass.sqlite
    ${CMAKE_CURRENT_SOURCE_DIR}/cyclus.rng.in
    ${CMAKE_CURRENT_SOURCE_DIR}/decayInfo.dat
  DESTINATION share
  COMPONENT core
  )

############################################################
############# end special configuration ####################
############################################################
  
#################################################################
######### build libcyclus #######################################
#################################################################

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR})

SET(
  CYCLUS_CORE_INCLUDE_DIRS ${CYCLUS_CORE_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  PARENT_SCOPE
  )

SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/query_engine.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/relax_ng_validator.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/xml_file_loader.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/xml_parser.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/xml_query_engine.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/builder.cc  
  ${CMAKE_CURRENT_SOURCE_DIR}/building_manager.cc  
  ${CMAKE_CURRENT_SOURCE_DIR}/commodity.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/commodity_producer.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/commodity_producer_manager.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/comp_math.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/composition.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/context.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/cyc_arithmetic.cc  
  ${CMAKE_CURRENT_SOURCE_DIR}/csv_back.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/decayer.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/enrichment.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/env.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/error.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/event.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/event_manager.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/l_matrix.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/logger.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/market_player.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/market_player_manager.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/mass_table.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/res_tracker.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/supply_demand_manager.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/sqlite_back.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/sqlite_db.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/symbolic_function_factories.cc  
  ${CMAKE_CURRENT_SOURCE_DIR}/symbolic_functions.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/timer.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/uniform_taylor.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/generic_resource.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/material.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/mat_buff.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/mat_query.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/resource_buff.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_module.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/facility_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/inst_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/market_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/message.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/region_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/resource.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/transaction.cc
  )

INSTALL(FILES
  query_engine.h 
  relax_ng_validator.h 
  xml_file_loader.h
  xml_parser.h
  xml_query_engine.h
  any.hpp
  blob.h
  builder.h
  building_manager.h
  commodity.h
  commodity_producer.h
  commodity_producer_manager.h
  comp_math.h
  csv_back.h
  cyc_arithmetic.h
  cyc_limits.h
  cyclus.h
  decayer.h
  enrichment.h
  env.h
  error.h
  event.h
  event_backend.h
  event_manager.h
  hdf5_back.h
  intrusive_base.h
  l_matrix.h
  logger.h
  market_player.h
  market_player_manager.h
  mass_table.h
  res_tracker.h
  supply_demand.h
  supply_demand_manager.h
  symbolic_function_factories.h
  symbolic_functions.h
  timer.h
  uniform_taylor.h
  use_matrix_lib.h
  sqlite_back.h
  sqlite_db.h
  composition.h
  context.h
  generic_resource.h
  material.h
  mat_buff.h
  mat_query.h
  resource.h
  resource_buff.h
  communicator.h
  dynamic_module.h
  facility_model.h
  inst_model.h
  market_model.h
  message.h
  model.h
  region_model.h
  suffix.h
  time_agent.h
  transaction.h
  unix_helper_functions.h
  version.h
  windows_helper_functions.h
  DESTINATION include/cyclus
  COMPONENT core
  )

ADD_SUBDIRECTORY(cyclopts)

ADD_LIBRARY( cycluscore ${CYCLUS_CORE_SRC} )

SET_TARGET_PROPERTIES( cycluscore 
  PROPERTIES 
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
  )

TARGET_LINK_LIBRARIES( cycluscore dl ${LIBS} )

INSTALL(
  TARGETS cycluscore
  LIBRARY DESTINATION lib
  COMPONENT cycluscore
  )

#################################################################
######### end build libcyclus ###################################
#################################################################


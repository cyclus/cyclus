###############################################################################
#################################### begin google test ########################
###############################################################################

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

FIND_PACKAGE(Threads)
IF(CMAKE_USE_PTHREADS_INIT)  # The pthreads library is available.
  SET(cxx_base_flags "${cxx_base_flags} -DGTEST_HAS_PTHREAD=1")
ENDIF()

# Build libgtest
SET( GTest
  "${CMAKE_CURRENT_SOURCE_DIR}/GoogleTest/gtest/gtest-all.cc"
  )

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")

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

INSTALL(FILES GoogleTest/gtest/gtest.h
  DESTINATION include/gtest
  COMPONENT testing
  )

###############################################################################
##################################### end google test #########################
###############################################################################

###############################################################################
################################### begin cyclus testing ######################
###############################################################################

INCLUDE_DIRECTORIES(${CYCLUS_CORE_INCLUDE_DIRS})

ADD_SUBDIRECTORY(test_modules)

SET(
  CYCLUS_CORE_TEST_INCLUDE_DIRS ${CYCLUS_CORE_TEST_INCLUDE_DIRS}
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${GTEST_INCLUDE_DIRS}"
  PARENT_SCOPE
  )

FILE(GLOB cc_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
SET(CYCLUS_CORE_TEST_SOURCE ${CYCLUS_CORE_SRC} ${cc_files})

ADD_SUBDIRECTORY(toolkit)
ADD_SUBDIRECTORY(cli)

FILE(GLOB header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB test_agents "${CMAKE_CURRENT_SOURCE_DIR}/test_modules/*.h")
INSTALL(FILES ${header_files}
  DESTINATION include/cyclus
  COMPONENT testing
  )

INSTALL(FILES ${test_agents}
  DESTINATION include/cyclus/test_modules
  COMPONENT testing
  )

FILE(GLOB agent_test_files "${CMAKE_CURRENT_SOURCE_DIR}/*agent_tests.cc")
INSTALL(FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/agent_tests.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/facility_tests.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/institution_tests.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/region_tests.cc"
  DESTINATION share/cyclus
  COMPONENT testing
  )

# read tests after building the driver, and add them to ctest
set( tgt "cyclus_unit_tests" )
add_custom_command(TARGET ${tgt}
  POST_BUILD
  COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/generate_gtest_macros.py"
          "--executable=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${tgt}"
          "--output=${CYCLUS_BINARY_DIR}/CTestTestfile.cmake"
  COMMENT "adding tests from ${tgt}"
  DEPENDS
  VERBATIM
  )

###############################################################################
#################################### end cyclus testing #######################
###############################################################################

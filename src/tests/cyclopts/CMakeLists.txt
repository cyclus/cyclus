set ( CYCLUS_TEST_CORE 
  # --
  ${CMAKE_CURRENT_SOURCE_DIR}/cbc_solver_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/function_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/variable_tests.cc
  PARENT_SCOPE)

FILE(GLOB cyclopts_header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${cyclopts_header_files}
  DESTINATION include/cyclopts
  COMPONENT testing
  )
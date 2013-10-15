set ( CYCLUS_CORE_TEST_SOURCE ${CYCLUS_CORE_TEST_SOURCE}
  # --
  ${CMAKE_CURRENT_SOURCE_DIR}/cbc_solver_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/function_tests.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/variable_tests.cc
  PARENT_SCOPE
  )

FILE(GLOB optim_header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${optim_header_files}
  DESTINATION include/optim
  COMPONENT testing
  )
# Add any new cyclus core source files to this list
SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/cbc_solver.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/function.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/solver.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/solver_interface.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/variable.cc
  PARENT_SCOPE)

INSTALL(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/cbc_solver.h
  ${CMAKE_CURRENT_SOURCE_DIR}/limits.h
  ${CMAKE_CURRENT_SOURCE_DIR}/function.h
  ${CMAKE_CURRENT_SOURCE_DIR}/solver.h
  ${CMAKE_CURRENT_SOURCE_DIR}/solver_interface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/variable.h
  DESTINATION include
  COMPONENT core
  )
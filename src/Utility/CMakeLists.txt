# Add any new cyclus core source files to this list
SET(CYCLUS_CORE ${CYCLUS_CORE} 
  ${CMAKE_CURRENT_SOURCE_DIR}/BookKeeper.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/CycException.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Database.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/DecayHandler.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Env.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/LMatrix.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/MassTable.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Table.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Timerr.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/UniformTaylor.cpp 
  PARENT_SCOPE)

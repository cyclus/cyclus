# Add any new cyclus core source files to this list
SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/BookKeeper.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/CycException.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Database.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/DecayHandler.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Env.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/LMatrix.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/MassTable.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/RecipeLogger.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Table.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Timer.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/UniformTaylor.cpp 
  PARENT_SCOPE)

install(FILES 
  BookKeeper.h
  CycException.h
  Database.h
  DecayHandler.h
  Env.h
  IntrusiveBase.h
  LMatrix.h
  Logger.h
  MassTable.h
  RecipeLogger.h
  Table.h
  Timer.h
  UniformTaylor.h
  UseMatrixLib.h
  DESTINATION cyclus/include
  )

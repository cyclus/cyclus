CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/mass.sqlite
  ${CYCLUS_BINARY_DIR}/share/mass.sqlite
  COPYONLY
  )
INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/mass.sqlite
  DESTINATION
    cyclus/share
  COMPONENT core
  )

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/decayInfo.dat
  ${CYCLUS_BINARY_DIR}/share/decayInfo.dat
  COPYONLY
  )
INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/decayInfo.dat
  DESTINATION 
    cyclus/share
  COMPONENT core
  )

SET(cyclus_install_dir ${CMAKE_INSTALL_PREFIX}/cyclus)
SET(cyclus_build_dir ${CYCLUS_BINARY_DIR})

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/Env.cpp.in
  ${CMAKE_CURRENT_SOURCE_DIR}/Env.cpp
  @ONLY
  )
  
# Add any new cyclus core source files to this list
SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/BookKeeper.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/BuildingManager.cpp  
  ${CMAKE_CURRENT_SOURCE_DIR}/CycException.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Database.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/DecayHandler.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Env.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/LMatrix.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/MassTable.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/RecipeLogger.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/SupplyDemandManager.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Table.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Timer.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/UniformTaylor.cpp 
  PARENT_SCOPE)

INSTALL(FILES 
  BookKeeper.h
  BuildingManager.h
  CycException.h
  Database.h
  DecayHandler.h
  Env.h
  IntrusiveBase.h
  LMatrix.h
  Logger.h
  MassTable.h
  RecipeLogger.h
  SupplyDemand.h
  SupplyDemandManager.h
  SymbolicFunctions.h
  Table.h
  Timer.h
  UniformTaylor.h
  UseMatrixLib.h
  DESTINATION cyclus/include
  COMPONENT core
  )


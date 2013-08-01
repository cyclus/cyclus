CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/mass.sqlite
  ${CYCLUS_BINARY_DIR}/share/mass.sqlite
  COPYONLY
  )
INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/mass.sqlite
  DESTINATION share
  COMPONENT core
  )

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/decayInfo.dat
  ${CYCLUS_BINARY_DIR}/share/decayInfo.dat
  COPYONLY
  )
INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/decayInfo.dat
  DESTINATION share
  COMPONENT core
  )

SET(cyclus_install_dir ${CMAKE_INSTALL_PREFIX})
SET(cyclus_build_dir ${CYCLUS_BINARY_DIR})

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/Env.cpp.in
  ${CMAKE_CURRENT_SOURCE_DIR}/Env.cpp
  @ONLY
  )
  
# Add any new cyclus core source files to this list
SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/Builder.cpp  
  ${CMAKE_CURRENT_SOURCE_DIR}/BuildingManager.cpp  
  ${CMAKE_CURRENT_SOURCE_DIR}/Commodity.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CommodityProducer.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/CommodityProducerManager.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/CycArithmetic.cpp  
  ${CMAKE_CURRENT_SOURCE_DIR}/csv_back.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/CycException.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/DecayHandler.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Enrichment.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Env.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Event.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/EventManager.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/LMatrix.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/MarketPlayer.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/MarketPlayerManager.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/MassTable.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Prototype.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/RecipeLibrary.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/SupplyDemandManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sqlite_back.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/SqliteDb.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/SymbolicFunctionFactories.cpp  
  ${CMAKE_CURRENT_SOURCE_DIR}/SymbolicFunctions.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/Timer.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/UniformTaylor.cpp 
  PARENT_SCOPE)

INSTALL(FILES 
  any.hpp
  blob.h
  Builder.h
  BuildingManager.h
  Commodity.h
  CommodityProducer.h
  CommodityProducerManager.h
  CycArithmetic.h
  csv_back.h
  CycException.h
  CycLimits.h
  DecayHandler.h
  Enrichment.h
  Env.h
  Event.h
  event_backend.h
  EventManager.h
  hdf5_back.h
  IntrusiveBase.h
  LMatrix.h
  Logger.h
  MarketPlayer.h
  MarketPlayerManager.h
  MassTable.h
  Prototype.h
  RecipeLibrary.h
  SupplyDemand.h
  SupplyDemandManager.h
  SymbolicFunctionFactories.h
  SymbolicFunctions.h
  Timer.h
  UniformTaylor.h
  UseMatrixLib.h
  sqlite_back.h
  SqliteDb.h
  DESTINATION include
  COMPONENT core
  )


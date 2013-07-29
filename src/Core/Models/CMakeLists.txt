
# Debian installs useful LibXML2 files to /usr/include/libxml2/libxml
FIND_PACKAGE( LibXml2 REQUIRED )
INCLUDE_DIRECTORIES ("${LIBXML2_INCLUDE_DIR}")

# Add the core model files to the core source
SET( CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/ConverterModel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DynamicModule.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/FacilityModel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/InstModel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MarketModel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Message.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Model.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/RegionModel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StubCommModel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StubModel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StubTimeAgent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Transaction.cpp
  PARENT_SCOPE 
  )

SET( TestSource ${ModelTestSource} PARENT_SCOPE )

INSTALL(FILES 
  Communicator.h
  ConverterModel.h
  DynamicModule.h
  FacilityModel.h
  InstModel.h
  MarketModel.h
  Message.h
  Model.h
  RegionModel.h
  StubCommModel.h
  StubModel.h
  StubTimeAgent.h
  TimeAgent.h
  Transaction.h
  DESTINATION include
  COMPONENT core
  )

add_all_subdirs()

SET( TestSource ${ModelTestSource} PARENT_SCOPE )


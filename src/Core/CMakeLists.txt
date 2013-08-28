
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Config
  ${CMAKE_CURRENT_SOURCE_DIR}/Models
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
  ${CMAKE_CURRENT_SOURCE_DIR}/cyc_arithmetic.cc  
  ${CMAKE_CURRENT_SOURCE_DIR}/csv_back.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/decay_handler.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/enrichment.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/env.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/error.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/event.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/event_manager.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/l_matrix.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/logger.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/market_player.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/market_player_manager.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/mass_table.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/prototype.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/recipe_library.cc 
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
  ${CMAKE_CURRENT_SOURCE_DIR}/converter_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_module.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/facility_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/inst_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/market_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/message.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/region_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/resource.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_comm_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/stub_time_agent.cc
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
  csv_back.h
  cyc_arithmetic.h
  cyc_limits.h
  decay_handler.h
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
  prototype.h
  recipe_library.h
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
  generic_resource.h
  material.h
  mat_buff.h
  mat_query.h
  resource.h
  resource_buff.h
  communicator.h
  converter_model.h
  dynamic_module.h
  facility_model.h
  inst_model.h
  market_model.h
  message.h
  model.h
  region_model.h
  stub_comm_model.h
  stub_model.h
  stub_time_agent.h
  time_agent.h
  transaction.h
  DESTINATION include
  COMPONENT core
  )

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
  ${CMAKE_CURRENT_SOURCE_DIR}/env.cpp.in
  ${CMAKE_CURRENT_SOURCE_DIR}/env.cpp
  @ONLY
  )

ADD_SUBDIRECTORY(cyclopts)
ADD_SUBDIRECTORY(Config)
ADD_SUBDIRECTORY(Models)
ADD_LIBRARY(cycluscore ${CYCLUS_CORE_SRC})

SET_TARGET_PROPERTIES( cycluscore 
  PROPERTIES 
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
  )

INSTALL(TARGETS cycluscore
  LIBRARY DESTINATION lib
  COMPONENT cycluscore
  )

SET( TestSource ${TestSource} PARENT_SCOPE )


SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} PARENT_SCOPE)
target_link_libraries(cycluscore dl ${Boost_FILESYSTEM_LIBRARY} 
  ${SQLITE3_LIBRARIES} ${LibXML++_LIBRARIES} 
  ${COIN_LIBRARIES} ${HDF5_LIBRARIES} ${HDF5_CXX_LIBRARIES})

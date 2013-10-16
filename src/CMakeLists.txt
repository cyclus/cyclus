
#################################################################
############# special file configuration ########################
#################################################################

CONFIGURE_FILE(suffix.h.in ${CMAKE_CURRENT_SOURCE_DIR}/suffix.h @ONLY)

EXECUTE_PROCESS(COMMAND git describe --tags OUTPUT_VARIABLE core_version OUTPUT_STRIP_TRAILING_WHITESPACE)
CONFIGURE_FILE(version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h @ONLY)

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/mass.sqlite
  ${CYCLUS_BINARY_DIR}/share/mass.sqlite
  COPYONLY
  )

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/decayInfo.dat
  ${CYCLUS_BINARY_DIR}/share/decayInfo.dat
  COPYONLY
  )

SET(cyclus_install_dir ${CMAKE_INSTALL_PREFIX})
SET(cyclus_build_dir ${CYCLUS_BINARY_DIR})
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/env.cc.in
  ${CMAKE_CURRENT_SOURCE_DIR}/env.cc
  @ONLY
  )

INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/mass.sqlite
    ${CMAKE_CURRENT_SOURCE_DIR}/cyclus.rng.in
    ${CMAKE_CURRENT_SOURCE_DIR}/decayInfo.dat
  DESTINATION share
  COMPONENT core
  )

############################################################
############# end special configuration ####################
############################################################
  
#################################################################
######### build libcyclus #######################################
#################################################################

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

SET(
  CYCLUS_CORE_INCLUDE_DIRS ${CYCLUS_CORE_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  PARENT_SCOPE
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
  ${CMAKE_CURRENT_SOURCE_DIR}/context.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/cyc_arithmetic.cc  
  ${CMAKE_CURRENT_SOURCE_DIR}/csv_back.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/decayer.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/enrichment.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/env.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/error.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/event.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/event_manager.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/l_matrix.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/logger.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/market_player.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/market_player_manager.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/mass_table.cc
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
  ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_module.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/facility_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/inst_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/market_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/message.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/region_model.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/resource.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/transaction.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/cbc_solver.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/function.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/solver.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/solver_interface.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/variable.cc
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
  comp_math.h
  csv_back.h
  cyc_arithmetic.h
  cyc_limits.h
  cyclus.h
  decayer.h
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
  context.h
  generic_resource.h
  material.h
  mat_buff.h
  mat_query.h
  resource.h
  resource_buff.h
  communicator.h
  dynamic_module.h
  facility_model.h
  inst_model.h
  market_model.h
  message.h
  model.h
  region_model.h
  suffix.h
  time_agent.h
  transaction.h
  unix_helper_functions.h
  version.h
  windows_helper_functions.h
  cbc_solver.h
  function.h
  solver.h
  solver_interface.h
  variable.h
  DESTINATION include/cyclus
  COMPONENT core
  )

ADD_LIBRARY( cycluscore ${CYCLUS_CORE_SRC} )

SET_TARGET_PROPERTIES( cycluscore 
  PROPERTIES 
  INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
  )

TARGET_LINK_LIBRARIES( cycluscore dl ${LIBS} )

INSTALL(
  TARGETS cycluscore
  LIBRARY DESTINATION lib
  COMPONENT cycluscore
  )

#################################################################
######### end build libcyclus ###################################
#################################################################


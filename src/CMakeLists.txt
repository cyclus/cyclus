
#################################################################
############# special file configuration ########################
#################################################################

CONFIGURE_FILE(suffix.h.in "${CMAKE_CURRENT_SOURCE_DIR}/suffix.h" @ONLY)

EXECUTE_PROCESS(COMMAND git describe --tags OUTPUT_VARIABLE core_version OUTPUT_STRIP_TRAILING_WHITESPACE)
CONFIGURE_FILE(version.cc.in "${CMAKE_CURRENT_SOURCE_DIR}/version.cc" @ONLY)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5")
    message("-- Found '${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5'")
else()
    message("-- Downloading http://regtests.fuelcycle.org/cyclus_nuc_data.h5 to "
            "'${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5'...")
    file(DOWNLOAD "http://regtests.fuelcycle.org/cyclus_nuc_data.h5" 
         "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5")
    message("-- ...download complete!")
endif()

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5"
  "${CYCLUS_BINARY_DIR}/share/cyclus_nuc_data.h5"
  COPYONLY
  )

SET(cyclus_install_dir "${CMAKE_INSTALL_PREFIX}")
SET(cyclus_build_dir "${CYCLUS_BINARY_DIR}")
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/env.cc.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/env.cc"
  @ONLY
  )

INSTALL(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus.rng.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus-flat.rng.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/cyclus_nuc_data.h5"
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
  "${CMAKE_CURRENT_SOURCE_DIR}"
  )

SET(
  CYCLUS_CORE_INCLUDE_DIRS ${CYCLUS_CORE_INCLUDE_DIRS}
  "${CMAKE_CURRENT_SOURCE_DIR}"
  PARENT_SCOPE
  )

SET(CYCLUS_CORE_SRC ${CYCLUS_CORE_SRC} 
  "${CMAKE_CURRENT_SOURCE_DIR}/builder.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/building_manager.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/cbc_solver.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/coin_helpers.cc" 
  "${CMAKE_CURRENT_SOURCE_DIR}/commodity.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/commodity_producer.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/commodity_producer_manager.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/commodity_recipe_context.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/comp_math.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/composition.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/context.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/csv_back.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/cyc_arithmetic.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/datum.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/db_init.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/decayer.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/dynamic_module.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/enrichment.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/env.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/error.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/exchange_graph.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/facility.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/function.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/product.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/greedy_preconditioner.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/greedy_solver.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/hdf5_back.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/institution.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/l_matrix.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/logger.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/mat_query.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/material.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/agent.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prog_solver.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prog_translator.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/pyne.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/infile_tree.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/recorder.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/region.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/relax_ng_validator.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/res_tracker.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/resource.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/resource_buff.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/sim_init.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/solver.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/solver_factory.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/solver_interface.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/sqlite_back.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/sqlite_db.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/supply_demand_manager.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/symbolic_function_factories.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/symbolic_functions.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/timer.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/uniform_taylor.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/variable.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/version.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/xml_file_loader.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/xml_flat_loader.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/xml_parser.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/OsiCbcSolverInterface.cpp"
  )

INSTALL(FILES
  any.hpp
  OsiCbcSolverInterface.hpp
  bid.h
  bid_portfolio.h
  blob.h
  builder.h
  building_manager.h
  capacity_constraint.h
  cbc_solver.h
  coin_helpers.h
  commodity.h
  commodity_producer.h
  commodity_producer_manager.h
  commodity_recipe_context.h
  comp_math.h
  composition.h
  context.h
  csv_back.h
  cyc_arithmetic.h
  cyc_limits.h
  cyc_std.h
  cyclus.h
  datum.h
  db_init.h
  decayer.h
  dynamic_module.h
  enrichment.h
  env.h
  error.h
  exchange_context.h
  exchange_graph.h
  exchange_manager.h
  exchange_solver.h 
  exchange_translator.h
  exchange_translation_context.h
  facility.h
  function.h
  product.h
  greedy_solver.h
  greedy_preconditioner.h
  hdf5_back.h
  query_backend.h
  institution.h
  intrusive_base.h
  l_matrix.h
  logger.h
  mat_query.h
  material.h
  agent.h
  prog_solver.h 
  prog_translator.h 
  pyne.h
  infile_tree.h 
  rec_backend.h
  recorder.h
  region.h
  relax_ng_validator.h 
  request.h
  request_portfolio.h
  res_tracker.h
  resource.h
  resource_buff.h
  resource_exchange.h
  sim_init.h
  solver.h
  solver_interface.h
  solver_factory.h
  sqlite_back.h
  sqlite_db.h
  state_wrangler.h
  suffix.h
  supply_demand.h
  supply_demand_manager.h
  symbolic_function_factories.h
  symbolic_functions.h
  time_listener.h
  timer.h
  trade.h
  trade_executor.h
  trader.h
  trader_management.h
  uniform_taylor.h
  unix_helper_functions.h
  use_matrix_lib.h
  variable.h
  version.h
  windows_helper_functions.h
  xml_file_loader.h
  xml_flat_loader.h
  xml_parser.h
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


#ifndef CYCLUS_SRC_CYCLUS_H_
#define CYCLUS_SRC_CYCLUS_H_

// For Doxygen mainpage
/*!
 * \mainpage Cyclus API Reference
 *
 * Welcome to the Cyclus API reference! Below are some helpful links for learning more:
 *   - Cyclus Homepage: https://fuelcycle.org
 *   - GitHub repository: https://github.com/cyclus/cyclus
 *   - Kernel developer guide: https://fuelcycle.org/kernel
 *   - Archetype developer guide: https://fuelcycle.org/arche
 * 
 */


// platform needs to be before all other includes
#include "platform.h"

#include "bid.h"
#include "bid_portfolio.h"
#include "capacity_constraint.h"
#include "comp_math.h"
#include "composition.h"
#include "context.h"
extern "C" {
#include "cram.hpp"
}
#include "cyc_arithmetic.h"
#include "cyc_limits.h"
#include "cyc_std.h"
#include "datum.h"
#include "db_init.h"
#include "discovery.h"
#include "exchange_graph.h"
#include "exchange_translation_context.h"
#include "env.h"
#include "error.h"
#include "facility.h"
#include "product.h"
#include "institution.h"
#include "logger.h"
#include "material.h"
#include "mock_sim.h"
#include "agent.h"
#include "pyhooks.h"
#include "pyne.h"
#include "query_backend.h"
#include "infile_tree.h"
#include "recorder.h"
#include "region.h"
#include "request.h"
#include "request_portfolio.h"
#include "resource.h"
#include "state_wrangler.h"
#include "time_listener.h"
#include "trade.h"
#include "trader.h"
#include "version.h"

#include "toolkit/builder.h"
#if CYCLUS_HAS_COIN
#include "toolkit/building_manager.h"
#endif
#include "toolkit/matl_buy_policy.h"
#include "toolkit/matl_sell_policy.h"
#include "toolkit/commodity.h"
#include "toolkit/commodity_producer.h"
#include "toolkit/commodity_producer_manager.h"
#include "toolkit/commodity_recipe_context.h"
#include "toolkit/enrichment.h"
#include "toolkit/infile_converters.h"
#include "toolkit/metadata.h"
#include "toolkit/mat_query.h"
#include "toolkit/position.h"
#include "toolkit/res_buf.h"
#include "toolkit/res_manip.h"
#include "toolkit/res_map.h"
#include "toolkit/supply_demand_manager.h"
#include "toolkit/symbolic_function_factories.h"
#include "toolkit/symbolic_functions.h"
#include "toolkit/timeseries.h"

// Undefines isnan from pyne
#ifdef isnan
  #undef isnan
#endif

#endif  // CYCLUS_SRC_CYCLUS_H_

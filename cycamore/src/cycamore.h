#ifndef CYCAMORE_SRC_CYCAMORE_H_
#define CYCAMORE_SRC_CYCAMORE_H_

// These includes must come before others.
#include "cyclus.h"
#include "cycamore_version.h"

#include "batch_reactor.h"
#include "batch_reactor_tests.h"
#include "deploy_inst.h"
#include "enrichment.h"
#include "enrichment_tests.h"
#if CYCLUS_HAS_COIN
#include "growth_region.h"
#include "growth_region_tests.h"
#endif
#include "inpro_reactor.h"
#include "inpro_reactor_tests.h"
#include "manager_inst.h"
#include "manager_inst_tests.h"
#include "sink.h"
#include "sink_tests.h"
#include "source.h"
#include "source_tests.h"

#endif  // CYCAMORE_SRC_CYCAMORE_H_

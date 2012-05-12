# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( GreedyMarket  GreedyMarket.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} GreedyMarket )
SET(MarketTestSource ${MarketTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/GreedyMarketTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/GreedyMarket.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Market/GreedyMarket/GreedyMarket.rng
${CMAKE_CURRENT_BINARY_DIR}/GreedyMarket.rng
COPYONLY
)

SET(RNG_INCLUDES ${RNG_INCLUDES} 
  "<include href='../Models/Market/GreedyMarket/GreedyMarket.rng'/>"
  PARENT_SCOPE)
SET(MKT_REFS ${MKT_REFS}
  "<ref name='GreedyMarket'/>"
  PARENT_SCOPE)

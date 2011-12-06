# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( GreedyMarket  GreedyMarket.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} GreedyMarket )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Market/GreedyMarket/GreedyMarket.rng
${CMAKE_CURRENT_BINARY_DIR}/GreedyMarket.rng
COPYONLY
)

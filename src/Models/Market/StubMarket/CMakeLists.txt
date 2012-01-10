# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubMarket  StubMarket.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubMarket )
SET(MarketTestSource ${MarketTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/StubMarketTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/StubMarket.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Market/StubMarket/StubMarket.rng
${CMAKE_CURRENT_BINARY_DIR}/StubMarket.rng
COPYONLY
)

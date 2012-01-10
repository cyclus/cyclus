# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( NullMarket  NullMarket.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} NullMarket )
SET(MarketTestSource ${MarketTestSource} 
  ${CMAKE_CURRENT_SOURCE_DIR}/NullMarketTests.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/NullMarket.cpp 
  PARENT_SCOPE)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Market/NullMarket/NullMarket.rng
${CMAKE_CURRENT_BINARY_DIR}/NullMarket.rng
COPYONLY
)

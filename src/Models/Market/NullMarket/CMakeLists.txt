# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( NullMarket  NullMarket.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} NullMarket )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Market/NullMarket/NullMarket.rng
${CMAKE_CURRENT_BINARY_DIR}/NullMarket.rng
COPYONLY
)

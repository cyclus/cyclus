# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubMarket  StubMarket.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubMarket )
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Models/Market/StubMarket/StubMarket.rng
${CMAKE_CURRENT_BINARY_DIR}/StubMarket.rng
COPYONLY
)

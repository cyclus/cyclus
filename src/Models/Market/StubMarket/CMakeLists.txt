# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubMarket  StubMarket.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubMarket )

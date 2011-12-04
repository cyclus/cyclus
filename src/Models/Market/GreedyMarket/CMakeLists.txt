# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( GreedyMarket  GreedyMarket.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} GreedyMarket )

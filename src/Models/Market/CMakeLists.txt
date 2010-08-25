# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( GreedyMarket MODULE GreedyMarket.cpp)
ADD_LIBRARY( NullMarket MODULE NullMarket.cpp)
ADD_LIBRARY( StubMarket MODULE StubMarket.cpp)

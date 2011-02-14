# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( ConverterMarket MODULE ConverterMarket.cpp)
ADD_LIBRARY( GreedyMarket MODULE GreedyMarket.cpp)
ADD_LIBRARY( NullMarket MODULE NullMarket.cpp)
ADD_LIBRARY( StubMarket MODULE StubMarket.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  ConverterMarket
  GreedyMarket
  NullMarket
  StubMarket
  )

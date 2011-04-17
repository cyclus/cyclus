# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( ConverterMarket  ConverterMarket.cpp)
ADD_LIBRARY( GreedyMarket  GreedyMarket.cpp)
ADD_LIBRARY( NullMarket  NullMarket.cpp)
ADD_LIBRARY( StubMarket  StubMarket.cpp)
SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  ConverterMarket
  GreedyMarket
  NullMarket
  StubMarket
  )

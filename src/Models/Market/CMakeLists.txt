# Build the cyclus executable from the CYCLUS_SRC source files
ADD_SUBDIRECTORY( ConverterMarket )
ADD_SUBDIRECTORY( GreedyMarket )
ADD_SUBDIRECTORY( NullMarket )
ADD_SUBDIRECTORY( StubMarket )

SET(ModelTestSource ${ModelTestSource} ${MarketTestSource} PARENT_SCOPE)

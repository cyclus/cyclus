# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( ConverterMarket  ConverterMarket.cpp)
SET( CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} ConverterMarket )

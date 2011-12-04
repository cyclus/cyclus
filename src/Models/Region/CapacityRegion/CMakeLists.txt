# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( CapacityRegion  CapacityRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} CapacityRegion )

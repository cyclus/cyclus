# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( NullRegion  NullRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} NullRegion )

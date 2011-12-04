# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( BuildRegion  BuildRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} BuildRegion )

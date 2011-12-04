# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( StubRegion  StubRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES} StubRegion )

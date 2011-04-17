# Build the cyclus executable from the CYCLUS_SRC source files
ADD_LIBRARY( NullRegion  NullRegion.cpp )
ADD_LIBRARY( StubRegion  StubRegion.cpp )
ADD_LIBRARY( BuildRegion  BuildRegion.cpp )

SET(CYCLUS_LIBRARIES ${CYCLUS_LIBRARIES}
  NullRegion
  StubRegion
  BuildRegion
  )
